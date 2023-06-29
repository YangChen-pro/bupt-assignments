#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "protocol.h"
#include "datalink.h"

#define MAX_PKT 256                 // ���Ĵ�С
#define MAX_SEQ 63                  // ������
#define NR_BUFS ((MAX_SEQ + 1) / 2) // ����ռ��С
#define DATA_TIMER 5000             // ֡��ʱʱ��
#define ACK_TIMER 280              // ack��ʱʱ��

#define inc(k)       \
    if (k < MAX_SEQ) \
        k = k + 1;   \
    else             \
        k = 0

typedef unsigned char frame_kind;
typedef unsigned int seq_nr;

// ֡�����ݽṹ
typedef struct
{
    frame_kind kind;             // ֡������
    unsigned char ack;           // ȷ�ϵ�֡�����
    unsigned char seq;           // ֡���
    unsigned char data[MAX_PKT]; // ֡����
    unsigned int padding;        // CRCУ��
} FRAME;

// �������ݽṹ
typedef struct
{
    unsigned char data[MAX_PKT]; // ������
} PACKET;

static bool no_nak = true;      // �Ƿ��Ѿ����͹�NAK
static int phl_ready = 0;       // ������Ƿ����

// �ж�����Ƿ�����a<=b<=c(ѭ��)
static bool between(seq_nr a, seq_nr b, seq_nr c)
{
    return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

// ��֡����У���ֶΣ�����֡���͸������
static void put_frame(unsigned char* frame, int len)
{
    *(unsigned int*)(frame + len) = crc32(frame, len);
    send_frame(frame, len + 4);
    phl_ready = 0;
}

// ����������һ��data,ack,����nak֡
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
            no_nak = false; // �Ѿ����͹�NAK��
            dbg_frame("Send NAK %d\n", frame_expected);
        }
        else // ack
        {
            dbg_frame("Send ACK %d\n", s.ack);
        }
        put_frame((unsigned char*)&s, 2);
    }
    // ��֡�������赥����ack֡
    stop_ack_timer();
}

int main(int argc, char** argv)
{
    seq_nr ack_expected;       // ���ʹ��ڵ��½�
    seq_nr next_frame_to_send; // ���ʹ��ڵ��Ͻ�+1
    seq_nr frame_expected;     // ���մ��ڵ��½�
    seq_nr too_far;            // ���մ��ڵ��Ͻ�+1
    int i;                     // ��������index
    FRAME r;                   // ���յ���֡
    PACKET out_buf[NR_BUFS];   // ���ʹ���
    PACKET in_buf[NR_BUFS];    // ���ܴ���
    bool arrived[NR_BUFS];     // ���մ��ڵ�bit map
    seq_nr nbuffered;          // ���ʹ����Ѿ����˶���
    int event;                 // �¼�����
    int arg;                   // ��ʱ��ʱ���ı��
    int len;                   // ֡����

    len = 0;
    ack_expected = 0;       // �����յ���ACK
    next_frame_to_send = 0; // ��һ�����͵�֡�����
    frame_expected = 0;
    too_far = NR_BUFS;
    nbuffered = 0; // ��ʼû�а�������
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
            get_packet(out_buf[next_frame_to_send % NR_BUFS].data);                   // ץȡһ���µ����ݰ�
            send_data_frame(FRAME_DATA, next_frame_to_send, frame_expected, out_buf); // ����֡
            inc(next_frame_to_send);
            break;

        case PHYSICAL_LAYER_READY:
            phl_ready = 1;
            break;

        case FRAME_RECEIVED:
            len = recv_frame((unsigned char*)&r, sizeof r);

            // �յ�����֡
            if (len < 5 || crc32((unsigned char*)&r, len) != 0)
            {
                dbg_event("**** Receiver Error, Bad CRC Checksum ****\n");
                // �յ�����֡����û�з���NAK������NAK
                if (no_nak)
                    send_data_frame(FRAME_NAK, 0, frame_expected, out_buf);
                break;
            }

            // �յ�������ȷ��֡ACK
            if (r.kind == FRAME_ACK)
                dbg_frame("Recv ACK  %d\n", r.ack);

            // �յ�����֡
            if (r.kind == FRAME_DATA)
            {
                dbg_frame("Recv DATA %d %d, ID %d\n", r.seq, r.ack, *(short*)r.data);
                // �յ���֡���������յ���֡��û�з���NAK
                if (r.seq != frame_expected && no_nak)
                    send_data_frame(FRAME_NAK, 0, frame_expected, out_buf);
                else
                    start_ack_timer(ACK_TIMER);
                if (between(frame_expected, r.seq, too_far) && arrived[r.seq % NR_BUFS] == false)
                {
                    arrived[r.seq % NR_BUFS] = true;
                    // len=PKT_LEN+3, len=len+4, len��PKT_LEN�Ļ�����+7, ���԰���=len-7
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

            // �յ�NAK
            if ((r.kind == FRAME_NAK) && between(ack_expected, (r.ack + 1) % (MAX_SEQ + 1), next_frame_to_send))
            {
                dbg_frame("Recv NAK %d\n", (r.ack + 1) % (MAX_SEQ + 1));
                send_data_frame(FRAME_DATA, (r.ack + 1) % (MAX_SEQ + 1), frame_expected, out_buf);
            }

            // �ۼ�ȷ��
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
