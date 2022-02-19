#ifndef PROTO_H__
#define PROTO_H__
#include <stdint.h>
#define RCVPORT "6666"

#define NAMESIZE 11

struct msg_st
{
    unsigned char name[NAMESIZE];
    unsigned int math;
    unsigned int chinese;
};

#endif