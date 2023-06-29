#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "protocol.h"
#include "datalink.h"

#define MAX_PKT 256                 // 包的大小
#define MAX_SEQ 63                  // 最大序号
#define NR_BUFS ((MAX_SEQ + 1) / 2) // 缓存空间大小
#define DATA_TIMER 5000             // 帧超时时间
#define ACK_TIMER 280              // ack超时时间

#define inc(k)       \
    if (k < MAX_SEQ) \
        k = k + 1;   \
    else             \
        k = 0

typedef unsigned char frame_kind;
typedef unsigned int seq_nr;

// 帧的数据结构
typedef struct
{
    frame_kind kind;             // 帧的类型
    unsigned char ack;           // 确认的帧的序号
    unsigned char seq;           // 帧序号
    unsigned char data[MAX_PKT]; // 帧数据
    unsigned int padding;        // CRC校验
} FRAME;

// 包的数据结构
typedef struct
{
    unsigned char data[MAX_PKT]; // 包数据
} PACKET;

static bool no_nak = true;      // 是否已经发送过NAK
static int phl_ready = 0;       // 物理层是否就绪

// 判断序号是否满足a<=b<=c(循环)
static bool between(seq_nr a, seq_nr b, seq_nr c)
{
    return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

// 给帧加入校验字段，并将帧发送给物理层
static void put_frame(unsigned char* frame, int len)
{
    *(unsigned int*)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}

// 构建并发送一个data,ack,或者nak帧
static void send_data_frame(frame_kind fk, seq_nr frame_nr, seq_nr frame_expected, PACKET buffer[])
{
    FRAME s;
    s.kind = fk;
    s.seq = frame_nr;
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
    if (fk == FRAME_DATA)
    {
        memcpy(s.data, buffer[frame_nr % NR_BUFS].data, PKT_LEN);
        dbg_frame("Send DATA %d %d, ID %d\n", s.seq, s.ack, *(short*)s.data);
        put_frame((unsigned char*)&s, 3 + PKT_LEN);
        start_timer(frame_nr % NR_BUFS, DATA_TIMER);
    }
    else
    {
        if (fk == FRAME_NAK)
        {
            no_nak = false; // 已经发送过NAK了
            dbg_frame("Send NAK %d\n", frame_expected);
        }
        else // ack
        {
            dbg_frame("Send ACK %d\n", s.ack);
        }
        put_frame((unsigned char*)&s, 2);
    }
    // 有帧发送无需单独的ack帧
    stop_ack_timer();
}

int main(int argc, char** argv)
{
    seq_nr ack_expected;       // 发送窗口的下界
    seq_nr next_frame_to_send; // 发送窗口的上界+1
    seq_nr frame_expected;     // 接收窗口的下界
    seq_nr too_far;            // 接收窗口的上界+1
    int i;                     // 缓冲区的index
    FRAME r;                   // 接收到的帧
    PACKET out_buf[NR_BUFS];   // 发送窗口
    PACKET in_buf[NR_BUFS];    // 接受窗口
    bool arrived[NR_BUFS];     // 接收窗口的bit map
    seq_nr nbuffered;          // 发送窗口已经用了多少
    int event;                 // 事件类型
    int arg;                   // 超时计时器的编号
    int len;                   // 帧长度

    len = 0;
    ack_expected = 0;       // 期望收到的ACK
    next_frame_to_send = 0; // 下一个发送的帧的序号
    frame_expected = 0;
    too_far = NR_BUFS;
    nbuffered = 0; // 初始没有包被缓存
    for (i = 0; i < NR_BUFS; i++)
        arrived[i] = false;

    protocol_init(argc, argv);
    lprintf("Designed by ziliangzhang, build: " __DATE__ "  "__TIME__
        "\n");

    disable_network_layer();

    for (;;)
    {
        event = wait_for_event(&arg);

        switch (event)
        {
        case NETWORK_LAYER_READY:
            nbuffered = nbuffered + 1;
            get_packet(out_buf[next_frame_to_send % NR_BUFS].data);                   // 抓取一个新的数据包
            send_data_frame(FRAME_DATA, next_frame_to_send, frame_expected, out_buf); // 传输帧
            inc(next_frame_to_send);
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = 1;
            break;

        case FRAME_RECEIVED:
            len = recv_frame((unsigned char*)&r, sizeof r);

            // 收到错误帧
            if (len < 5 || crc32((unsigned char*)&r, len) != 0)
            {
                dbg_event("**** Receiver Error, Bad CRC Checksum ****\n");
                // 收到错误帧，且没有发过NAK，则发送NAK
                if (no_nak)
                    send_data_frame(FRAME_NAK, 0, frame_expected, out_buf);
                break;
            }

            // 收到单独的确认帧ACK
            if (r.kind == FRAME_ACK)
                dbg_frame("Recv ACK  %d\n", r.ack);

            // 收到数据帧
            if (r.kind == FRAME_DATA)
            {
                dbg_frame("Recv DATA %d %d, ID %d\n", r.seq, r.ack, *(short*)r.data);
                // 收到的帧不是期望收到的帧且没有发过NAK
                if (r.seq != frame_expected && no_nak)
                    send_data_frame(FRAME_NAK, 0, frame_expected, out_buf);
                else
                    start_ack_timer(ACK_TIMER);
                if (between(frame_expected, r.seq, too_far) && arrived[r.seq % NR_BUFS] == false)
                {
                    arrived[r.seq % NR_BUFS] = true;
                    // len=PKT_LEN+3, len=len+4, len在PKT_LEN的基础上+7, 所以包长=len-7
                    memcpy(in_buf[r.seq % NR_BUFS].data, r.data, len - 7);
                    while (arrived[frame_expected % NR_BUFS])
                    {
                        put_packet(in_buf[frame_expected % NR_BUFS].data, len - 7);
                        no_nak = true;
                        arrived[frame_expected % NR_BUFS] = false;
                        inc(frame_expected);
                        inc(too_far);
                        start_ack_timer(ACK_TIMER);
                    }
                }
            }

            // 收到NAK
            if ((r.kind == FRAME_NAK) && between(ack_expected, (r.ack + 1) % (MAX_SEQ + 1), next_frame_to_send))
            {
                dbg_frame("Recv NAK %d\n", (r.ack + 1) % (MAX_SEQ + 1));
                send_data_frame(FRAME_DATA, (r.ack + 1) % (MAX_SEQ + 1), frame_expected, out_buf);
            }

            // 累计确认
            while (between(ack_expected, r.ack, next_frame_to_send))
            {
                nbuffered = nbuffered - 1;
                stop_timer(ack_expected % NR_BUFS);
                inc(ack_expected);
            }
            break;

        case DATA_TIMEOUT:
            dbg_event("---- DATA %d timeout ----\n", arg);
            if (!between(ack_expected, arg, next_frame_to_send))
                arg += NR_BUFS;
            send_data_frame(FRAME_DATA, arg, frame_expected, out_buf);
            break;

        case ACK_TIMEOUT:
            dbg_event("---- ACK %d timeout ----\n", (frame_expected + MAX_SEQ) % (MAX_SEQ + 1));
            send_data_frame(FRAME_ACK, 0, frame_expected, out_buf);
            break;
        }

        if (nbuffered < NR_BUFS && phl_ready)
            enable_network_layer();
        else
            disable_network_layer();
    }
}
