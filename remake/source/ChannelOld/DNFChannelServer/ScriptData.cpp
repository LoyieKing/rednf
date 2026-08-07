#include "ScriptData.h"
#include "GlobalInstance.h"

ScriptData::ScriptData()
{
    clear();
}

ScriptData::~ScriptData()
{
}

void ScriptData::clear()
{
    memcpy(&ip[0], "192.168.0.8", 0xc);
    tcp_port = 0x15b3;
    udp_port = 0x15b3;
    max_client = 0x96;
    strncpy(&channel_script_version[0], "0.0.0", 0x13);
    channel_script_version[0x13] = '\0';
    strncpy(&db_ip[0], "192.168.0.11", 0xf);
    db_ip[0xf] = '\0';
    strncpy(&db_name[0], "neople", 0x31);
    db_name[0x31] = '\0';
    strncpy(&db_id[0], "neople", 0x13);
    db_pwd[0x13] = '\0';
    strncpy(&db_pwd[0], "tkWh.", 0x13);
    db_pwd[0x13] = '\0';
}

template class GlobalInstance<GLOBAL>;
template class GlobalInstance<ScriptData>;
