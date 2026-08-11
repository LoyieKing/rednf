#include "Script.h"
#include "GlobalInstance.h"
#include "ScriptData.h"
#include <string.h>
#include <assert.h>

Script::Script()
{
}

Script::~Script()
{
    if (data != NULL)
    {
        delete data;
        data = NULL;
    }
}

bool Script::load(char* filename)
{
    data = new ScriptRawData("[__root__]", "title", 1);
#line 27 "Script.cpp"
    assert(data);
    memset(parent_tag, 0, 0x100);
    depth = 0;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }
    char buf[0x401];
    bool ret;
    for (;;)
    {
        ret = fgetln(fp, buf);
        parse(buf);
        if (!ret)
        {
            break;
        }
    }
    fclose(fp);
    return true;
}

bool Script::parse(char* line)
{
    remove_comment(line);
    get_key_val(line);
    return true;
}

bool Script::fgetln(FILE* fp, char* buf)
{
    int i = 0;
    while (i <= 0x3ff)
    {
        int code = fgetc(fp);
        if (feof(fp) != 0)
        {
            buf[i] = '\0';
            return false;
        }
        buf[i] = (char)code;
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            if ((0 < i) && (buf[i - 1] == '\r'))
            {
                buf[i - 1] = '\0';
                return true;
            }
            return true;
        }
        i = i + 1;
    }
    return true;
}

bool Script::remove_comment(char* line)
{
    int i = 0;
    while (i <= 0x3fd)
    {
        int n = 0;
        for (int j = 0; j < 2; j = j + 1)
        {
            if (line[i + j] == "//"[j])
            {
                n = n + 1;
            }
        }
        if (n == 2)
        {
            memset(line + i, 0, 0x400 - i);
            return true;
        }
        i = i + 1;
    }
    return false;
}

bool Script::get_key_val(char* line)
{
    tok.get_token(line, strlen(line));
    int n_tok = tok.get_n_token();
    switch (n_tok)
    {
    case 1:
        on_parent_tag(tok.get_context(0));
        break;
    case 2:
        on_keyval_tag(tok.get_context(0), tok.get_context(1));
        break;
    default:
        return false;
    }
    return true;
}

bool Script::on_parent_tag(char* key)
{
    ScriptRawData* s = new ScriptRawData(key, NULL, 0);
    if (s == NULL)
    {
        return false;
    }
    data->push_child(s);
    memset(parent_tag, 0, 0x100);
    strncpy(parent_tag, key, 0xff);
    return true;
}

bool Script::on_keyval_tag(char* key, char* val)
{
    ScriptRawData* s = new ScriptRawData(key, val, strlen(val));
    if (s == NULL)
    {
        return false;
    }
#line 162 "Script.cpp"
    assert(strlen(parent_tag));
    data->push_child(parent_tag, s);
    return true;
}

bool Script::get_server_section()
{
    char* val = NULL;
    val = data->get_data("[server]", "max_client");
    if (val != NULL)
    {
        G_ScriptData()->max_client = atoi(val);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "this_ip");
    if (val != NULL)
    {
        strncpy(G_ScriptData()->ip, val, 0x11);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "this_tcp_port");
    if (val != NULL)
    {
        G_ScriptData()->tcp_port = (unsigned short)atoi(val);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "this_udp_port");
    if (val != NULL)
    {
        G_ScriptData()->udp_port = (unsigned short)atoi(val);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "bridge_ip");
    if (val != NULL)
    {
        strncpy(G_ScriptData()->bridge_ip, val, 0x11);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "bridge_port");
    if (val != NULL)
    {
        G_ScriptData()->bridge_port = atoi(val);
    }
    else
    {
        return false;
    }
    val = data->get_data("[server]", "id");
    if (val != NULL)
    {
        G_ScriptData()->id = atoi(val);
    }
    else
    {
        return false;
    }
    return true;
}

bool Script::parse_channel_script()
{
    bool ret = get_server_section();
    if (!ret)
    {
        puts("Script interpret error in Channel-[Server]");
        return false;
    }
    destroy_raw_script();
    return true;
}

void Script::destroy_raw_script()
{
    if (data != NULL)
    {
        delete data;
        data = NULL;
    }
}

template class GlobalInstance<GLOBAL>;
template class GlobalInstance<Script>;
