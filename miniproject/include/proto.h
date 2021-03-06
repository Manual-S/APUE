#ifndef PROTO_H_
#define PROTO_H_

#define DEFAULT_MGROUP "224.2.2.2"  //default multicast group
#define DEFAULT_RCVPORT "6666"  //port
#define CHANNR 200//channel number

#define LISTCHNID 0//list channel
#define MINCHNID 1//minimum channel id
#define MAXCHIND (MINCHNID + CHANNR - 1)//maximum channel id

#define MSG_CHANNEL_MAX (65536 - 20 - 8)//20:ip package head, 8:udp package head
#define MAX_DATA (MSG_CHANNEL_MAX - sizeof(chnid_t))

#define MSG_LIST_MAX (65536 -20 -8)
#define MAX_ENTRY (MSG_LIST_MAX-sizeof(chnid_t))
#include "site_type.h"
//each channel content data structure
struct msg_channel_st
{
    chnid_t chnid;  /*must between [MINCHNID, MAXCHNID]*/
    uint8_t data[1];
}__attribute__((packed));//do not align

/*
1 music xxxxxxxxxxxxxxxxxxxx
2 sport xxxxxxxxxxxxxxxxxxxx
3 xxxx xxxxxx
...
*/
//each list content
struct msg_listentry_st
{
    chnid_t chnid;
    uint16_t len;
    uint8_t desc[1];
}__attribute__((packed));//do not align


//channel list struct
struct msg_list_st
{
    chnid_t chnid;  /*must be LISTCHNID*/
    struct msg_listentry_st entry[1];

}__attribute__((packed));//do not align

#endif