// df_relay_r — Script（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdlib.h>
#include <string.h>

#include "Script.h"
#include "ScriptRawData.h"
#include "ScriptData.h"
#include "Helper.h"

// ---- Script ----

Script::Script()
    : data(0), fp(0), depth(0)
{
    memset(parent_tag, 0, 0x100);
}

Script::~Script()
{
    if (data != 0)
    {
        delete data;
        data = 0;
    }
}

bool Script::load(char* path)
{
    data = new ScriptRawData("[__root__]", "title", 1);
    memset(parent_tag, 0, 0x100);
    depth = 0;
    fp = fopen(path, "rb");
    if (fp == 0)
    {
        return 0;
    }
    char line[1025];
    bool r;
    for (;;)
    {
        r = fgetln(fp, line);
        parse(line);
        if (!r)
        {
            break;
        }
    }
    fclose(fp);
    return 1;
}

bool Script::fgetln(FILE* f, char* buf)
{
    int i = 0;
    while (i <= 0x3ff)
    {
        int c = fgetc(f);
        if (feof(f) != 0)
        {
            buf[i] = 0;
            return 0;
        }
        buf[i] = (char)c;
        if (buf[i] == '\n')
        {
            buf[i] = 0;
            if ((0 < i) && (buf[i - 1] == '\r'))
            {
                buf[i - 1] = 0;
                return 1;
            }
            return 1;
        }
        i++;
    }
    return 1;
}

int Script::parse(char* line)
{
    remove_comment(line);
    get_key_val(line);
    return 1;
}

int Script::remove_comment(char* line)
{
    int i = 0;
    while (i <= 0x3fd)
    {
        int matched = 0;
        for (int j = 0; j <= 1; j++)
        {
            if (line[i + j] == "//"[j])
            {
                matched++;
            }
        }
        if (matched == 2)
        {
            memset(line + i, 0, 0x400 - i);
            return 1;
        }
        i++;
    }
    return 0;
}

int Script::get_key_val(char* line)
{
    tok.get_token(line, strlen(line));
    int n = tok.get_n_token();
    switch (n)
    {
    case 1:
        on_parent_tag(tok.get_context(0));
        break;
    case 2:
        on_keyval_tag(tok.get_context(0), tok.get_context(1));
        break;
    default:
        return 0;
    }
    return 1;
}

bool Script::on_parent_tag(char* tag)
{
    ScriptRawData* sd = new ScriptRawData(tag, 0, 0);
    if (sd == 0)
    {
        return false;
    }
    data->push_child(sd);
    memset(parent_tag, 0, 0x100);
    strncpy(parent_tag, tag, 0xff);
    parent_tag[0xff] = 0;
    return true;
}

bool Script::on_keyval_tag(char* key, char* value)
{
    ScriptRawData* sd = new ScriptRawData(key, value, strlen(value));
    if (sd == 0)
    {
        return false;
    }
    data->push_child(parent_tag, sd);
    return true;
}

void Script::destroy_raw_script()
{
    if (data != 0)
    {
        delete data;
        data = 0;
    }
}

bool Script::get_server_section()
{
    char* v = 0;
    v = data->get_data("[server]", "max_client");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mRelayNum = atoi(v);
    v = data->get_data("[server]", "this_ip");
    if (v == 0)
    {
        return 0;
    }
    strncpy((char*)G_ScriptData() + 8, v, 0x10);
    G_ScriptData()->mServerIpA[16] = 0;
    v = data->get_data("[server]", "this_tcp_port");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mPortTcp = (unsigned short)atoi(v);
    v = data->get_data("[server]", "this_udp_port");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mPortUdp = (unsigned short)atoi(v);
    v = data->get_data("[server]", "this_pvp_channel");
    if (v == 0)
    {
        G_ScriptData()->mReservedA = 0;
    }
    else
    {
        G_ScriptData()->mReservedA = (unsigned short)atoi(v);
    }
    return 1;
}

bool Script::get_aradauth_section()
{
    char* v = 0;
    v = data->get_data("[aradauth]", "udp_port_to_monitor");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mReservedB = (unsigned short)atoi(v);
    v = data->get_data("[aradauth]", "monitor_ip");
    if (v == 0)
    {
        return 0;
    }
    strncpy((char*)G_ScriptData() + 0x1e, v, 0x10);
    G_ScriptData()->mServerIpB[16] = 0;
    v = data->get_data("[aradauth]", "monitor_port");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mReservedC = (unsigned short)atoi(v);
    v = data->get_data("[aradauth]", "relay_index");
    if (v == 0)
    {
        return 0;
    }
    G_ScriptData()->mReservedD = (unsigned short)atoi(v);
    return 1;
}

bool Script::parse_channel_script()
{
    bool r = get_server_section();
    if (!r)
    {
        puts("Script interpret error in Channel-[Server]");
        return 0;
    }
    r = get_aradauth_section();
    if (!r)
    {
        puts("[!] Scirpt Doesn't use in [aradauth] - Not Use Monitor Auth");
        G_ScriptData()->mFlag = 0;
    }
    destroy_raw_script();
    return 1;
}
