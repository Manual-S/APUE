#include <errno.h>
#include "server_conf.h"

static pthread_t tid_list;
static int nr_list_ent;
static struct mlib_listentry_st* list_ent;

static void* thr_list(void* p)
{

}

int thr_list_create(struct mlib_listentry_st *listp,int nr_ent)
{

}
int thr_list_destroy(void)
{
    return 0;
}