#ifndef CLIENT_H__
#define CLIENT_H__

struct client_conf_st
{
    char *rcvport;
    char *mgroup;
    char *player_cmd;
};

#define DEFAULT_PLAYERCMD " - "  // Mplayer的位置
extern struct client_conf_st client_conf;

#endif