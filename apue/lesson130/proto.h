#ifndef PROTO_H__
#define PROTO_H__
#define RCVPORT "6666"

struct msg_st
{
    unsigned char name[NAMESIZE];
    unsigned int math;
    unsigned int chinese;
};


#define NAMESIZE 11
#endif