#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "Script.h"

namespace nsl {

Script::Script()
    : tok(), mLoadScriptLock(), mParentNames(), mChildNames(), mChildValues()
{
}

Script::~Script()
{
}

bool Script::load(char* filename)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLoadScriptLock);
    char buf[1025];
    memset(buf, 0, 0x401);
    Clear();
    depth = 0;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }
    bool ret;
    do
    {
        ret = fgetln(fp, buf);
        parse(buf);
    } while (ret);
    fclose(fp);
    return true;
}

bool Script::fgetln(FILE* fp, char* buf)
{
    int i = 0;
    while (true)
    {
        if (i > 0x3ff)
        {
            return true;
        }
        int code = fgetc(fp);
        if (feof(fp) != 0)
        {
            break;
        }
        buf[i] = (char)code;
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            if ((i > 0) && (buf[i - 1] == '\r'))
            {
                buf[i - 1] = '\0';
            }
            return true;
        }
        i = i + 1;
    }
    buf[i] = '\0';
    return false;
}

bool Script::parse(char* line)
{
    remove_comment(line);
    if (!get_key_val(line))
    {
        puts("\xC5\xE4\xC5\xAB\xC0\xBB \xBE\xF2\xBE\xEE \xBF\xC0\xB4\xC2\xB5\xA5 \xBD\xC7\xC6\xD0 \xC7\xCF\xBF\xB4\xBD\xBF");
        exit(1);
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
    size_t size = strlen(line);
    tok.get_token(line, size);
    int n_tok = tok.get_n_token();
    if (n_tok == 1)
    {
        mParentNames.push_back(std::string(tok.get_context(0)));
        mChildNames.push_back(std::vector<std::string>());
        mChildValues.push_back(std::vector<std::string>());
        tok.free_token(0);
    }
    else if (n_tok == 2)
    {
        mChildNames[mParentNames.size() - 1].push_back(std::string(tok.get_context(0)));
        mChildValues[mParentNames.size() - 1].push_back(std::string(tok.get_context(1)));
        tok.free_token(0);
        tok.free_token(1);
    }
    else if (n_tok != 0)
    {
        return false;
    }
    return true;
}

bool Script::get_sections()
{
    uint i = 0;
    do
    {
        if (mParentNames.size() <= i)
        {
            return true;
        }
        uint j = 0;
        while (true)
        {
            if (mChildNames[i].size() <= j)
            {
                break;
            }
            if (mChildValues[i][j].c_str() == 0)
            {
                return false;
            }
            j = j + 1;
        }
        i = i + 1;
    } while (true);
}

void Script::Clear()
{
    mParentNames.clear();
    mChildNames.clear();
    mChildValues.clear();
}

} // namespace nsl
