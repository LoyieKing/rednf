#include "Script.h"
#include "GlobalInstance.h"
#include "ScriptData.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

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
    do
    {
        ret = fgetln(fp, buf);
        parse(buf);
    } while (ret == true);
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
        int eof = feof(fp);
        if (eof != 0)
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
        int j = 0;
        while (j <= 1)
        {
            if (line[i + j] == "//"[j])
            {
                n = n + 1;
            }
            j = j + 1;
        }
        if (n != 2)
        {
            i = i + 1;
        }
        else
        {
            memset(line + i, 0, 0x400 - i);
            return true;
        }
    }
    return false;
}

bool Script::get_key_val(char* line)
{
    size_t size = strlen(line);
    tok.get_token(line, size);
    int n_tok = tok.get_n_token();
    if (n_tok == 1)
    {
        char* key = tok.get_context(0);
        on_parent_tag(key);
    }
    else
    {
        if (n_tok != 2)
        {
            return false;
        }
        char* val = tok.get_context(1);
        char* key = tok.get_context(0);
        on_keyval_tag(key, val);
    }
    return true;
}

bool Script::on_parent_tag(char* key)
{
    ScriptRawData* s = new ScriptRawData(key, NULL, 0);
    if (s != NULL)
    {
        data->push_child(s);
        memset(parent_tag, 0, 0x100);
        strncpy(parent_tag, key, 0xff);
    }
    return s != NULL;
}

bool Script::on_keyval_tag(char* key, char* val)
{
    ScriptRawData* s = new ScriptRawData(key, val, strlen(val));
    if (s == NULL)
    {
        return false;
    }
    if (parent_tag[0] == '\0')
    {
        assert(strlen(parent_tag));
    }
    data->push_child(parent_tag, s);
    return true;
}

bool Script::get_server_section()
{
    char* val = NULL;
    val = data->get_data("[server]", "max_client");
    if (val == 0)
    {
        return false;
    }
    G_ScriptData()->max_client = atoi(val);
    val = data->get_data("[server]", "this_ip");
    if (val != 0)
    {
        strncpy(G_ScriptData()->ip, val, 0x11);
        val = data->get_data("[server]", "this_tcp_port");
        if (val != 0)
        {
            G_ScriptData()->tcp_port = (unsigned short)atoi(val);
            val = data->get_data("[server]", "this_udp_port");
            if (val != 0)
            {
                G_ScriptData()->udp_port = (unsigned short)atoi(val);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Script::get_db_section()
{
    char* val = NULL;
    val = data->get_data("[DB]", "db_ip");
    if (val == 0)
    {
        return false;
    }
    strncpy(G_ScriptData()->db_ip, val, 0xf);
    val = data->get_data("[DB]", "db_name");
    if (val != 0)
    {
        strncpy(G_ScriptData()->db_name, val, 0x31);
        val = data->get_data("[DB]", "db_id");
        if (val != 0)
        {
            strncpy(G_ScriptData()->db_id, val, 0x13);
            val = data->get_data("[DB]", "db_pwd");
            if (val != 0)
            {
                strncpy(G_ScriptData()->db_pwd, val, 0x3f);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Script::parse_channel_script()
{
    bool ret = get_server_section();
    if (ret == false)
    {
        puts("Script interpret error in Channel-[Server]");
        return false;
    }
    ret = get_db_section();
    if (ret == false)
    {
        puts("Script interpret error in Channel-[DB]");
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
