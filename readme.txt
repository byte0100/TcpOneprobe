agent端启用：
sh startagent.sh <port> <-log>
后两者均为可选参数 默认为 port:9090 -log
log日志 文件名：testclient.log

关闭agent:
sh killagent.sh

testclient_fork 启动
编译
gcc testclient_fork.c -o testclientfork
./testclientfork <server_ip> <port> <datasize> <nchildren>
<datasize>每轮测量数据总大小

make clean
make
即可完成编译