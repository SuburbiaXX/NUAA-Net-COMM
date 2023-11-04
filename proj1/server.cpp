#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <unordered_map>

using namespace std;

const int BUF_SIZE = 1024;
const char* MULTICAST_GROUP = "224.0.0.1"; // 组播地址
const int MULTICAST_PORT = 12345; // 组播端口
int fd; // 套接字描述符

// 用于保存客户端信息的结构体
struct ClientInfo {
    string key; // 使用 IP 和 PID 的组合作为键
    string ip;
    pid_t pid;
    int ttl; // TTL 倒计时
};

pthread_mutex_t mutex_clients; // 互斥锁, 实现 clientList 的线程安全
unordered_map<string, ClientInfo> clientList; // 保存客户端信息的列表

void* check(void* arg) { // 检查客户端状态
    while (true) {
        sleep(10); // 每 10 秒检查一次客户端
        pthread_mutex_lock(&mutex_clients);
        // 遍历客户端列表，并且正常在线的输出客户端信息
        cout << "CHECK========================================CHECK" << endl;
        for (auto it = clientList.begin(); it != clientList.end(); ) {
            it->second.ttl -= 10; // 减少 TTL 10 秒，判断是否还在线
            if (it->second.ttl < 0) { // 如果 TTL 小于 0，删除客户端信息，否则，输出客户端信息
            // 这里小于等于 0 感觉都无所谓
                it = clientList.erase(it);
            } else {
                cout << "Client IP: " << it->second.ip << ", PID: " << it->second.pid << endl;
                ++it;
            }
        }
        cout << "CHECK========================================CHECK" << endl;
        pthread_mutex_unlock(&mutex_clients);
    }
    return nullptr;
}

void* receiveMessages(void* arg) { // 处理客户端发送的消息
    char recv_buf[BUF_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (true) {
        int recv_bytes = recvfrom(fd, recv_buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (recv_bytes < 0) { // 如果接收失败，继续监听下一次消息
            perror("recvfrom");
            continue;
        }

        recv_buf[recv_bytes] = '\0'; // 确保字符串结束
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        string message(recv_buf); // 将接收到的消息转换为字符串以便下面截取处理
        if (message.find("IP:") != string::npos && message.find("PID:") != string::npos) { // 客户端的注册消息
            string ip = client_ip;
            string pid_str = message.substr(message.find("PID:") + 5);
            string key = ip + "-" + pid_str;
            pthread_mutex_lock(&mutex_clients);
            // 检查客户端是否已经在列表中
            if (clientList.find(key) == clientList.end()) { // 不在，添加到列表并欢迎新客户端
                cout << "========================================" << endl;
                cout << "Welcome client IP: " << ip << ", PID: " << pid_str << endl;
                cout << "========================================" << endl;
                clientList[key] = {key, ip, stoi(pid_str), 10};
            } else { // 已知客户端，重置 TTL
                clientList[key].ttl = 10;
            }
            pthread_mutex_unlock(&mutex_clients);
        } else { // 输出普通消息
            cout << "Received reply from " << client_ip << ":" << ntohs(client_addr.sin_port) << endl
                 << "\t\t" << recv_buf << endl << endl;
        }
    }
    return nullptr;
}

int main() {
    pthread_mutex_init(&mutex_clients, NULL); // 初始化互斥锁
    fd = socket(AF_INET, SOCK_DGRAM, 0); // 创建套接字
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    // 指定多播消息的网络接口
    in_addr localInterface;
    localInterface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &localInterface, sizeof(localInterface)) < 0) {
        perror("setsockopt(IP_MULTICAST_IF) failed");
        close(fd);
        return 1;
    }

    // 设置套接字选项，允许发送广播消息
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    cliaddr.sin_port = htons(MULTICAST_PORT);

    // check 线程
    pthread_t tid1;
    if (pthread_create(&tid1, NULL, check, NULL) != 0) {
        perror("pthread_create");
        close(fd);
        return 1;
    }

    // receiveMessages 线程
    pthread_t tid2;
    if (pthread_create(&tid2, NULL, receiveMessages, NULL) != 0) {
        perror("pthread_create");
        close(fd);
        return 1;
    }

    char buf[BUF_SIZE] = "Hello Multicast Group!"; // 广播的消息
    while (true) {
        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
            perror("sendto");
            break;
        }
        cout << "Sent message: " << buf << endl;
        sleep(1); // 每 1 秒发送一次消息
    }
    close(fd); // 关闭套接字
    return 0;
}
