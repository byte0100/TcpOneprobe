agent端启用：
sh startagent.sh <port> <-log>
后两者均为可选参数 默认为 port:9090 -log
log日志 文件名：testclient.log

关闭agent:
sh killagent.sh

testclient 启动
./testclient <server_ip> <port> <packetsize>
<packetsize>是指每个报文的数据域大小，不是总大小

make clean
make
即可完成编译