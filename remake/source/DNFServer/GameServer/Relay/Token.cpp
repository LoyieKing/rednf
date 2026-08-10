// df_relay_r — Token（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdlib.h>
#include <string.h>

#include "Token.h"

Token::Token()
{
    n_token = 0;
    memset(seps, 0, 50);
    seps[0] = ' ';
    seps[1] = '=';
    seps[2] = '\t';
    seps[3] = 1;
    seps[4] = 0;
    memset(m_token, 0, 0x400);
}

Token::~Token()
{
    for (int i = 0; i < n_token; i++)
    {
    }
}

bool Token::get_token(char* str, int size)
{
    if (size < 0)
    {
        return false;
    }
    char* dest = new char[size + 1];
    memset(dest, 0, size + 1);
    strncpy(dest, str, size);
    n_token = 0;
    char* token = strtok(dest, seps);
    while (token != 0 && n_token <= 0xff)
    {
        size_t len = strlen(token);
        if ((int)len > 0x800)
        {
            len = 0x800;
        }
        m_token[n_token] = new char[len + 1];
        memset(m_token[n_token], 0, len + 1);
        strncpy(m_token[n_token], token, len);
        n_token++;
        token = strtok(0, seps);
    }
    delete[] dest;
    return true;
}

char* Token::get_context(int idx0)
{
    return m_token[idx0];
}

void Token::free_token(int idx0)
{
}

int Token::get_n_token()
{
    return n_token;
}
