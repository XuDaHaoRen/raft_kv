# raft-kv

这是一个基于 raft 共识算法的 etcd 数据库，它基于 redis 实现了一种 k-v 存储方式，数据通信使用 asio 框架

## 如何运行

### Build
    
    git clone https://github.com/XuDaHaoRen/raft_kv.git
    mkdir -p raft-kv/build
    cd raft-kv/build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j8
    
### 运行集群

首先安装 [goreman](https://github.com/mattn/goreman), 它可以实现对进程，然后执行下面的指令

    goreman start
    
    
### 测试
install [redis-cli](https://github.com/antirez/redis), a redis console client.

    redis-cli -p 63791
    127.0.0.1:63791> set mykey myvalue
    OK
    127.0.0.1:63791> get mykey
    "myvalue"
    
先将 node 2 暂停，并且对 key 值进行更新：

    goreman run stop node2
    redis-cli -p 63791
    127.0.0.1:63791> set mykey new-value
    OK
    
重新将 node 2 启动并查看更新后的值

    redis-cli -p 63792
    127.0.0.1:63792> KEYS *
    1) "mykey"
    127.0.0.1:63792> get mykey
    "new-value"
    
### benchmark

    redis-benchmark -t set,get -n 100000 -p 63791
    
    ====== SET ======
      100000 requests completed in 1.35 seconds
      50 parallel clients
      3 bytes payload
      keep alive: 1
    
    96.64% <= 1 milliseconds
    99.15% <= 2 milliseconds
    99.90% <= 3 milliseconds
    100.00% <= 3 milliseconds
    73909.83 requests per second
    
    ====== GET ======
      100000 requests completed in 0.95 seconds
      50 parallel clients
      3 bytes payload
      keep alive: 1
    
    99.95% <= 4 milliseconds
    100.00% <= 4 milliseconds
    105485.23 requests per second
    
    
    
    
    
    
    

