//
// Created by 54766 on 2019/5/13.
//

#ifndef AGENTV2_TCPSERVER_H
#define AGENTV2_TCPSERVER_H

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <syslog.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#define  error_handling(message) {fputs(message,stderr);fputc('\n',stderr);exit(1);}

#endif //AGENTV2_TCPSERVER_H
