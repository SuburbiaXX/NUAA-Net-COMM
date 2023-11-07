#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NUMBER_OF_LAN 10
#define NUMBER_OF_PORTS 1024
#define HOST_NAME_MAX 255
#define MAX_OPEN_PORTS 1024

struct Address {
    char hostname[HOST_NAME_MAX];
    char address[INET_ADDRSTRLEN];
    int open_ports[MAX_OPEN_PORTS];
    int open_ports_count;
} addr[NUMBER_OF_LAN];

void TranAddr(char *ad) {
    struct in_addr ipv4addr;
    struct hostent *host;

    // 生成所有地址，并获取主机名，初始化 Address 结构体
    char *p = strrchr(ad, '.');
    ad = strndup(ad, p - ad);
    for (int i = 0; i < NUMBER_OF_LAN; ++i) {
        sprintf(addr[i].address, "%s.%d", ad, i + 1);
        inet_pton(AF_INET, addr[i].address, &ipv4addr);
        host = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
        strncpy(addr[i].hostname, (host != NULL) ? host->h_name : "Unknown", HOST_NAME_MAX-1);
        addr[i].hostname[HOST_NAME_MAX-1] = '\0';
        addr[i].open_ports_count = 0;
    }
}

void sockconnect(struct Address *a, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(a->address);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        a->open_ports[a->open_ports_count++] = port; // 记录开放端口
    }
    close(sockfd);
}

void *pth_main(void *arg) {
    // 避免多线程同时访问同一个地址，每个线程都有自己的 Address 结构体
    struct Address *a = (struct Address *)arg;
    for (int port = 1; port <= NUMBER_OF_PORTS; ++port) {
        sockconnect(a, port);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Base IP Address>\n", argv[0]);
        return 1;
    }

    TranAddr(argv[1]);

    pthread_t thread[NUMBER_OF_LAN];
    for (int i = 0; i < NUMBER_OF_LAN; ++i) {
        if (pthread_create(&thread[i], NULL, pth_main, &addr[i])) {
            perror("Error creating thread");
            continue;
        }
    }

    for (int i = 0; i < NUMBER_OF_LAN; ++i) {
        pthread_join(thread[i], NULL);
    }

    // 输出结果
    printf("%-*s\t%-*s\t开放端口\n", 15, "主机名", 15, "ip地址");
    for (int i = 0; i < NUMBER_OF_LAN; ++i) {
        printf("%-*s\t%-*s\t", 15, addr[i].hostname, 15, addr[i].address);
        if (addr[i].open_ports_count == 0) {
            printf("none\n");
        } else {
            for (int j = 0; j < addr[i].open_ports_count; ++j) {
                printf("'%d' ", addr[i].open_ports[j]);
            }
            printf("\n");
        }
    }
    return 0;
}