# 网络通信实现技术 2023
- 共有 5 个 project
## project 1 
- 基于IP组播(多播)的通信实现
- 内容：由一个进程向一个组播组发送报文，组播组中的相关进程接收报文，并将报文显示到屏幕上。要求服务器为客户加入组时为这个客户端创建一个进程。退出时撤销该进程。注意这里组播是局域网内部组播，不通过路由器转发，所以组播地址224.0.0.0-224.0.0.255。
### 大概思路
#### 客户端
1. 创建套接字：使用 socket() 函数创建一个 UDP 套接字，返回一个描述符
2. 绑定套接字：使用 bind() 函数将套接字绑定到本地 IP 地址和端口号上
3. 加入组播组：创建一个 ip_mreq 结构体，设置组播 IP 地址和接口地址，然后使用 setsockopt() 函数将套接字加入到组播组
4. 定义缓冲区和发送方地址：创建字符数组 buf 用于接收和发送消息，创建字符数组 sendaddrbuf 用于存储发送方的 IP 地址，创建 sendaddr 结构体用于存储发送方的地址信息
5. 进行通信循环：使用 recvfrom() 函数接收来自组播组的消息，并打印发送方的 IP 地址和端口号。然后根据接收到的消息内容，选择性地发送回复消息。循环执行这个过程，直到达到指定的测试次数
6. 关闭套接字：使用 close() 函数关闭套接字

#### 服务端
1. 创建套接字：使用 socket() 函数创建一个 UDP 套接字，返回一个文件描述符 fd
2. 设置组播属性：使用 inet_pton() 函数将组播 IP 地址转换为网络字节序，并使用 setsockopt() 函数设置组播属性
3. 定义缓冲区和发送方地址：创建字符数组 buf 用于发送和接收消息，创建字符数组 sendaddrbuf 用于存储发送方的 IP 地址，创建 sendaddr 结构体用于存储发送方的地址信息
4. 定义接收端地址信息：创建 cliaddr 结构体用于存储接收端的地址信息，包括地址族、端口号和 IP 地址
5. 创建检查线程：使用 pthread_create() 函数创建一个新线程，执行 check 函数
6. 进行通信循环：使用 sendto() 函数发送组播消息，并打印发送的消息内容。然后使用 recvfrom() 函数接收来自接收端的回复消息，并打印发送方的 IP 地址和端口号。根据接收到的消息内容，进行相应的处理，如欢迎新成员加入组播组、标记活跃状态、杀死进程等。循环执行这个过程，实现组播的发送和接收功能

### 实现情况
- 客户端中额外通过发送注册信息，以此来表示存活状态
- 服务端中通过维护列表来判断客户端新上线、活跃状态等
- 因为在本地 Ubuntu 环境中实现，利用多个终端模拟不同角色，客户端的 IP 是一致的，故引入了进程 PID 来作为区分客户端的手段
