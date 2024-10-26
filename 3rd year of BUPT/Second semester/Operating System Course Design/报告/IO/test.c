/**
 * @file test.c
 * @brief 这个程序演示了如何使用gettimeofday()函数来测量HTTP请求的响应时间和吞吐量。
 * 
 * 该程序创建了一个TCP客户端，该客户端连接到指定的IP地址和端口，并发送多个HTTP请求。
 * 它使用gettimeofday()函数来测量从发送第一个请求到接收最后一个响应所经过的时间。
 * 然后，它计算平均响应时间和吞吐量。
 * 
 * 使用方法: ./test [server_ip] [server_port] [num_requests]
 * 
 * @param server_ip 服务器的IP地址。
 * @param server_port 服务器的端口号。
 * @param num_requests 要发送的HTTP请求的数量。
 * 
 * @return 成功返回0，错误返回-1。
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/**
 * @brief 用法函数，打印程序的使用方法。
 * 
 * @param proc 程序的名称。
 */
static void Usage(const char *proc)
{
    printf("%s [server_ip] [server_port] [num_requests]\n", proc);
}

/**
 * @brief 主函数，程序的入口。
 * 
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 成功返回0，错误返回-1。
 */
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        Usage(argv[0]);
        return -1;
    }

    const char *server_ip = argv[1]; // 服务器的IP地址
    int server_port = atoi(argv[2]); // 服务器的端口号
    int num_requests = atoi(argv[3]); // 要发送的HTTP请求的数量
    int num_tests = 10; // 设置测试次数

    double total_elapsed_time = 0; // 总共经过的时间
    double total_throughput = 0; // 总吞吐量

    int t;
    for (t = 0; t < num_tests; t++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
        if (sock < 0)
        {
            perror("socket");
            return -1;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        server_addr.sin_addr.s_addr = inet_addr(server_ip);

        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) // 连接到服务器
        {
            perror("connect");
            return -1;
        }

        char request[] = "GET / HTTP/1.0\r\n\r\n"; // HTTP请求
        int request_size = strlen(request);

        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL); // 获取开始时间

        int i;
        for (i = 0; i < num_requests; i++) // 发送多个HTTP请求
        {
            if (write(sock, request, request_size) < 0) // 发送请求
            {
                perror("write");
                return -1;
            }

            char response[1024];
            ssize_t num_bytes = read(sock, response, sizeof(response) - 1); // 读取响应
            if (num_bytes < 0)
            {
                perror("read");
                return -1;
            }
            else if (num_bytes == 0)
            {
                // printf("Server closed the connection.\n");
                break;
            }

            response[num_bytes] = '\0';
            // printf("Response: %s\n", response);
        }

        gettimeofday(&end_time, NULL); // 获取结束时间
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0; // 计算经过的时间
        double throughput = num_requests / elapsed_time; // 计算吞吐量

        printf("经过时间: %.2f 秒\n", elapsed_time);
        printf("吞吐量: %.2f 请求/秒\n", throughput);

        total_elapsed_time += elapsed_time;
        total_throughput += throughput;

        close(sock); // 关闭套接字
    }

    double avg_elapsed_time = total_elapsed_time / num_tests; // 计算平均经过时间
    double avg_throughput = total_throughput / num_tests; // 计算平均吞吐量

    printf("平均经过时间: %.2f 秒\n", avg_elapsed_time);
    printf("平均吞吐量: %.2f 请求/秒\n", avg_throughput);

    return 0;
}