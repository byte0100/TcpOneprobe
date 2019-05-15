//
// Created by 54766 on 2019/5/13.
//

#ifndef AGENTV2_MSGFORMAT_H
#define AGENTV2_MSGFORMAT_H

// #pragma pack(1) 结构对齐的问题

#define     CONTROL_INIT        1
#define     CONTROL_GET         2
#define     CONTROL_ACK         3


typedef  struct MessageHeader
{
    unsigned char               messageHeader[4];   //协议头 区分不同协议 iSCSSI协议：“CC_I” other协议：“CC_O”
    short                       controlMask;        //操作码 区分同一协议不同命令
}MsgHeader;

#endif //AGENTV2_MSGFORMAT_H
