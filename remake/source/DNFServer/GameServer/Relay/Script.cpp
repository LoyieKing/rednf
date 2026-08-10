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
        ScriptRawData* sd = data;
        if (sd != 0)
        {
            delete sd;
        }
        data = 0;
    }
}

int Script::load(char* path)
{
    ScriptRawData* sd = new ScriptRawData("[__root__]", "title", 1);
    data = sd;
    memset(parent_tag, 0, 0x100);
    depth = 0;
    fp = fopen(path, "rb");
    if (fp == 0)
    {
        return 0;
    }
    char line[1025];
    int r;
    do
    {
        r = fgetln(fp, line);
        parse(line);
    } while (r == 1);
    fclose(fp);
    return 1;
}

int Script::fgetln(FILE* f, char* buf)
{
    int i = 0;
    while (1)
    {
        if (0x3ff < i)
        {
            return 1;
        }
        int c = fgetc(f);
        int eof = feof(f);
        if (eof != 0)
        {
            break;
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
    buf[i] = 0;
    return 0;
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
    while (1)
    {
        if (0x3fd < i)
        {
            return 0;
        }
        int matched = 0;
        for (int j = 0; j < 2; j++)
        {
            if (line[i + j] == "//"[j])
            {
                matched++;
            }
        }
        if (matched == 2)
        {
            break;
        }
        i++;
    }
    memset(line + i, 0, 0x400 - i);
    return 1;
}

int Script::get_key_val(char* line)
{
    tok.get_token(line, strlen(line));
    int n = tok.get_n_token();
    if (n == 1)
    {
        char* tag = tok.get_context(0);
        on_parent_tag(tag);
    }
    else
    {
        if (n != 2)
        {
            return 0;
        }
        char* key = tok.get_context(0);
        char* val = tok.get_context(1);
        on_keyval_tag(key, val);
    }
    return 1;
}

bool Script::on_parent_tag(char* tag)
{
    ScriptRawData* sd = new ScriptRawData(tag, 0, 0);
    if (sd != 0)
    {
        data->push_child(sd);
        memset(parent_tag, 0, 0x100);
        strncpy(parent_tag, tag, 0xff);
        parent_tag[0xff] = 0;
    }
    return sd != 0;
}

bool Script::on_keyval_tag(char* key, char* value)
{
    ScriptRawData* sd = new ScriptRawData(key, value, strlen(value));
    if (sd != 0)
    {
        data->push_child(parent_tag, sd);
    }
    return sd != 0;
}

void Script::destroy_raw_script()
{
    if (data != 0)
    {
        ScriptRawData* sd = data;
        if (sd != 0)
        {
            delete sd;
        }
        data = 0;
    }
}

int Script::get_server_section()
{
    char* v = data->get_data("[server]", "max_client");
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
    *(char*)((char*)G_ScriptData() + 0x18) = 0;
    v = data->get_data("[server]", "this_tcp_port");
    if (v == 0)
    {
        return 0;
    }
    *(unsigned short*)((char*)G_ScriptData() + 4) = (unsigned short)atoi(v);
    v = data->get_data("[server]", "this_udp_port");
    if (v == 0)
    {
        return 0;
    }
    *(unsigned short*)((char*)G_ScriptData() + 6) = (unsigned short)atoi(v);
    v = data->get_data("[server]", "this_pvp_channel");
    if (v == 0)
    {
        *(unsigned short*)((char*)G_ScriptData() + 0x1a) = 0;
    }
    else
    {
        *(unsigned short*)((char*)G_ScriptData() + 0x1a) = (unsigned short)atoi(v);
    }
    return 1;
}

int Script::get_aradauth_section()
{
    char* v = data->get_data("[aradauth]", "udp_port_to_monitor");
    if (v == 0)
    {
        return 0;
    }
    *(unsigned short*)((char*)G_ScriptData() + 0x1c) = (unsigned short)atoi(v);
    v = data->get_data("[aradauth]", "monitor_ip");
    if (v == 0)
    {
        return 0;
    }
    strncpy((char*)G_ScriptData() + 0x1e, v, 0x10);
    *(char*)((char*)G_ScriptData() + 0x2e) = 0;
    v = data->get_data("[aradauth]", "monitor_port");
    if (v == 0)
    {
        return 0;
    }
    *(unsigned short*)((char*)G_ScriptData() + 0x30) = (unsigned short)atoi(v);
    v = data->get_data("[aradauth]", "relay_index");
    if (v == 0)
    {
        return 0;
    }
    *(unsigned short*)((char*)G_ScriptData() + 0x32) = (unsigned short)atoi(v);
    return 1;
}

int Script::parse_channel_script()
{
    if (get_server_section() == 1)
    {
        if (get_aradauth_section() != 1)
        {
            puts("[!] Scirpt Doesn't use in [aradauth] - Not Use Monitor Auth");
            *(char*)((char*)G_ScriptData() + 0x34) = 0;
        }
        destroy_raw_script();
        return 1;
    }
    puts("Script interpret error in Channel-[Server]");
    return 0;
}
