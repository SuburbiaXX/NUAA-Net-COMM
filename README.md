# 网络通信实现技术 2023
- 共有 5 个 project
## project 1 
- 基于IP组播(多播)的通信实现
- 内容：由一个进程向一个组播组发送报文，组播组中的相关进程接收报文，并将报文显示到屏幕上。要求服务器为客户加入组时为这个客户端创建一个进程。退出时撤销该进程。注意这里组播是局域网内部组播，不通过路由器转发，所以组播地址224.0.0.0-224.0.0.255。
### 思路
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

### 情况
- 客户端中额外通过发送注册信息，以此来表示存活状态
- 服务端中通过维护列表来判断客户端新上线、活跃状态等
- 因为在本地 Ubuntu 环境中实现，利用多个终端模拟不同角色，客户端的 IP 是一致的，故引入了进程 PID 来作为区分客户端的手段

## project 2
- SCANNER 简单网络扫描程序实现
- 内容：用C语言编写一个扫描局域网内主机的程序。要求可以显示局域网内的主机名列表，IP地址列表，并可以显示哪些主机开放了哪些端口。

### 思路
- 通过输入的 IP 地址得到需要扫描的网段(但是这里并没有实现带有掩码的输入)
- 创建套接字，并通过多线程对每个 IP 地址的各个端口进行扫描
- 结果保存在 Address 的结构体中，然后格式化输出

### 情况
- 根据之前 [网络安全课设](https://github.com/SuburbiaXX/Cybersecurity) ，依照本次实验要求和思路进行了修改，思路其实是一致的

## project 3
- 利用 NS3 部署不同拓扑无线网络
- 内容：利用 NS3 部署一个星型无线网络（一个 AP，不少于 5 个接入点）、一个多跳无线网络（不少于 6 个网络节点）。并测量两种网络拓扑下的网络链接吞吐量，用图表表示。
### 思路
- 根据提示和网上参考代码，搭建两个拓扑网络
- 吞吐量的计算使用脚本对执行生成的 tr 文件进行处理，这里用到了 gawk 工具
- 绘制图表使用的 gnuplot 工具，对处理后的数据进行绘制
- xml 文件在 NetAnim 中进行可视化，模拟上可能没有那么直观
  - 这里使用另一种可视化办法，[参考博客自行安装依赖](https://blog.csdn.net/qq_51482354/article/details/131701427)
### 情况
- 参考了一堆代码，还是有些不懂的地方
- 非常感谢以下两位大佬的代码
  - [@FlyAndNotDown](https://github.com/FlyAndNotDown/ns3-work)
  - [@RMDE](https://github.com/RMDE/Telecommunication)

## project 4
- 利用 NS3 部署一个 LTE 网络，并绘制仿真的 Radio Environment Map
### 思路
- 根据提供的参数对 src/lte/example/lena-dual-stripe.cc 这个文件进行修改，使其符合要求，同时需要利用相关软件对 lte 网络情况进行可视化的表示
### 情况
- 修改情况参考如下 [CSDN文章](https://blog.csdn.net/Shmily1107/article/details/110919007)
- 修改参数后用 NS3 编译执行后会产生如下文件，即 proj4 下的文件，情况如下
  ```
  proj4
  |----buildings.txt  # 仿真的建筑物信息
  |----enbs.txt   # 仿真的基站信息
  |----lte.cc   # 修改 lena-dual-stripe.cc 后的文件，需要放在 scratch 目录下利用 waf 编译运行
  |----plot_script  # 用于绘制 Radio Environment Map 的脚本文件
  |----proj4.rem  # 编译运行 lte.cc 后生成的 rem 文件，其中包含网络不同位置的信号强度信息
  |----proj4.sh   # 用于执行 lte.cc 的脚本文件，简化输入流程
  |----ues.txt  # 仿真的用户信息
  ```
- 感谢大佬 [@Messiah](https://github.com/Messiahccgr) 的指导和帮助

## project 5
- 基于 proj4，实现 LTE 网络数据的统计
- 在 proj4 的基础上，实现对 LTE 网络中数据的统计，其中包括，统计用户当前接入基站的RSRP，SINR，吞吐量以及距离基站的距离
### 思路
- 需要先在 proj4 中的 lte.cc 下添加 `lteHelper->EnablePhyTraces();` 语句，启用 LTE 模块中物理层的跟踪功能，这样才使得仿真过程的交互可以被记录
- 然后执行 proj5.sh 脚本，其中已经写好了编译的相关参数，具体参数下面会提到
- 计算吞吐量
  - 在 lte.cc 编译完后生成的 ulrdatastats.txt  和 dlrdatastats.txt 文件分别表示的是上行链路（Uplink）和下行链路（Downlink）的RLC层统计数据，他们分别表示着从用户设备到基站和从基站到用户设备的数据传输情况
    - 计算的 python 脚本为 cal1.py，其中计算吞吐量的公式来由在 ns-3.30.1/src/lte/doc/source/lte-user.rst 中
    - 运行生成的 tu.txt 表示每个基站的吐量，tun.txt 表示每个基站的吞量
- 计算 RSRP 和 SINR 
  - 利用的是 dlrspsinr.txt 文件，这是用于记录下行链路（从基站到移动设备）的 RSRP 和 SINR 数据。其中 RSRP 表示的是接收信号强度参考功率，SINR 表示的是信噪比。这两个参数都是用来衡量信号质量的
    - 计算的 python 脚本为 cal2.py
    - 从生成的 RSRP-SINR-AVG.csv 文件中可以看到每个基站的平均 RSRP 和 SINR，转换成 dB
- 计算距离基站的距离
  - NS-3提供了获取节点位置的方法，只需要对于每个 UE，找到它所连接的 eNB，并计算它们之间的距离即可
  - 具体的实现在 lte.cc 中的 `Simulator::Run ();` 后，添加如下代码
    ```c++
    std::ofstream distanceFile;
    distanceFile.open("Distance.txt");
    // 获取 UE 和 eNB 节点
	    NodeContainer ueNodes ;
    NodeContainer enbNodes ;
    ueNodes.Add(homeUes);
    ueNodes.Add(macroUes);
    enbNodes.Add(homeEnbs);
    enbNodes.Add(macroEnbs);
    for (NodeContainer::Iterator ue = ueNodes.Begin (); ue != ueNodes.End (); ++ue)
    {
        Ptr<Node> ueNode = (*ue);
        Ptr<MobilityModel> ueMobility = ueNode->GetObject<MobilityModel>();
        // 获取 UE 的连接 eNB
        Ptr<LteUeNetDevice> ueLteDevice = ueNode->GetDevice(0)->GetObject<LteUeNetDevice>();
        if (ueLteDevice)
        {
            Ptr<LteEnbNetDevice> connectedEnb = ueLteDevice->GetTargetEnb();
            // 获取 eNB 的位置
            for (NodeContainer::Iterator enb = enbNodes.Begin (); enb != enbNodes.End (); ++enb)
            {
                Ptr<Node> enbNode = (*enb);
                if (enbNode->GetDevice(0) == connectedEnb)
                {
                    Ptr<MobilityModel> enbMobility = enbNode->GetObject<MobilityModel>();
                    double distance = ueMobility->GetDistanceFrom(enbMobility);
                    // 将距离信息写入文件
                    distanceFile << "UE ID: " << ueNode->GetId() << " connected to eNB ID: " << enbNode->GetId() << " Distance: " << distance << " meters" << std::endl;
                    break;
                }
            }
        }
    }
    ```
  - 生成的 Distance.txt 文件中可以看到每个 UE 距离所连接的 eNB 的距离
- proj5 文件夹情况
  ```
  proj5
  |----cal1.py  # 计算吞吐量的 python 脚本
  |----cal2.py  # 计算每个基站 RSRP 和 SINR 均值的 python 脚本
  |----Distance.txt  # 每个 UE 距离所连接的 eNB 的距离结果
  |----dlrdatastats.txt  # 下行链路的 RLC 层统计数据
  |----dlrspsinr.txt  # 下行链路的 RSRP 和 SINR 数据
  |----lte.cc  # 修改后的文件，需要放在 scratch 目录下利用 waf 编译运行
  |----proj5.sh  # 编译运行 lte.cc 的脚本文件
  |----RSRP-SINR-AVG.csv  # 每个基站的平均 RSRP 和 SINR
  |----tu.txt  # 每个基站的吐量
  |----tun.txt  # 每个基站的吞量
  |----ulrdatastats.txt  # 上行链路的 RLC 层统计数据
  |----ulrsinr.txt  # 上行链路的 SINR 数据
  ```
### 情况
- 感谢大佬 [@Messiah](https://github.com/Messiahccgr) 的指导和帮助