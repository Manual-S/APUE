#ifdef PROTO_H__
#define PROTO_H__

#include <stdint.h>
#include "site_type.h"

#define LISTCHNID 0  // 0号频道来广播节目单
#define DEFAULT_RCVPORT "6666"
#define DEFAULT_MGROUP ""

#define MSG_CHANNEL_MAX (65536-20-8)


#define MSG_LIST_MAX (65536-20-8)

// 每一个数据包的结构
struct msg_channel_st
{
    chnid_t chnid;
    uint8_t data[1];
};

struct msg_listentry_st
{
    chnid_t chnid;  // 频道号
    uint16_t len;  // 长度
    uint8_t desc[1];  // 频道描述
};

struct msg_list_st  
{
    chind_t chnid;
    struct msg_listentry_st entry[1];
};


#endif