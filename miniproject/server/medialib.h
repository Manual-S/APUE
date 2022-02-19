#ifndef MEDIALIB_H__
#define MEDIALIB_H__

struct media_listentry_st
{
    /* data */
    chnid_t chnid;
    char* desc;
};

int mlib_getchnlist(struct media_listentry_st **,int *);
int mlib_freechnlist(struct media_listentry_st *);
ssize_t mlib_readchn(chnid_t,void*,size_t);

#endif