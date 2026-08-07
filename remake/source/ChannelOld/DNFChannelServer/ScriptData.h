#ifndef SCRIPTDATA_H_
#define SCRIPTDATA_H_

#include "stdafx.h"

class ScriptData
{
public:
    ScriptData();
    ~ScriptData();
    void clear();

    int id;
    int bridge_port;
    char bridge_ip[17];
    char channel_script_version[20];
    int max_client;
    unsigned short tcp_port;
    unsigned short udp_port;
    char ip[17];
    char db_ip[16];
    char db_name[50];
    char db_id[20];
    char db_pwd[20];
    char pid_filename[257];
};

ScriptData* G_ScriptData();

#endif // SCRIPTDATA_H_
