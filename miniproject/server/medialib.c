#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include "medialib.h"

#define PATHSIZE 1024

extern struct server_conf_st server_conf;

struct channel_context_st
{
    chnid_t chnid;
    char *desc;
    glob_t mp3glob;
    int pos;
    int fd;
    off_t offset;
    mytbf_t *tbf;
};
static struct channel_context_st channel[CHNNR +1];
//
int mlib_getchnlist(struct media_listentry_st** result,int* resultnum)
{
    glob_t globres;
    char path[PATHSIZE];
    int errcode = 0;
    int i = 0;
    int num = 0;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;
    // 遍历数组 初始化
    for(i = 0;i < CHNNR + 1;i++)
    {
        channel[i].chnid = -1;
    }

    snprintf(paht,PATHSIZE,"%s/*",server_conf.media_dir);

    errcode = glob(path,0,NULL,&globres);
    if (errcode < 0)
    {
        return -1;
    }

    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if(ptr == NULL)
    {
        // 守护进程 要用系统日志来报错
        return -1;
    }

    for(i = 0;i <globres.gl_pathc;i++)
    {
        // globres.gl_pathv[i] -> "/var/media/ch1"
        path2entry(globres.gl_pathv[i]);
        num++;  
    }

    *result = ;
    *resultnum = num;

}
int mlib_freechnlist(struct media_listentry_st *);
ssize_t mlib_readchn(chnid_t,void*,size_t);