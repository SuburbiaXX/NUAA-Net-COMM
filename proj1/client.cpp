#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <netdb.h> 
#include <unistd.h>

using namespace std;

const int BUF_SIZE = 1024;
const char* MULTICAST_GROUP = "224.0.0.1";
const int MULTICAST_PORT = 12345;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // 创建套接字
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 允许多个套接字绑定到同一端口
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(sock);
        return 1;
    }

    // 设置套接字为广播模式
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MULTICAST_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) { // 绑定套接字
        perror("bind");
        close(sock);
        return 1;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) { // 加入组播组
        perror("setsockopt(IP_ADD_MEMBERSHIP) failed");
        close(sock);
        return 1;
    }

    // 获取客户端的 PID IP
    pid_t pid = getpid();
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1) {
        perror("gethostname");
        exit(1);
    }
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

    char buf[BUF_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    
    while (true) {
        // 接收消息
        int nbytes = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
        if (nbytes < 0) {
            perror("recvfrom");
            close(sock);
            return 1;
        }
        buf[nbytes] = '\0';
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, INET_ADDRSTRLEN);
        // 输出接收到的消息
        cout << "Received message from " << sender_ip << ":" << ntohs(sender_addr.sin_port) << endl << "------------------------------------------" << endl << "\t\t" << buf << endl << endl;
        // 发送注册信息
        snprintf(buf, BUF_SIZE, "IP: %s, PID: %d", IPbuffer, pid);
        if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&sender_addr, sender_addr_len) < 0) {
            perror("sendto");
            close(sock);
            return 1;
        }
        // 回复广播消息
        const char* reply_msg = "hello server";
        if (sendto(sock, reply_msg, strlen(reply_msg), 0, (struct sockaddr*)&sender_addr, sender_addr_len) < 0) {
            perror("sendto");
            close(sock);
            return 1;
        }
    }
    close(sock);
    return 0;
}
