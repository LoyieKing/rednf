#include <assert.h>
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

    char* temp = new char[size + 1];
    if (temp == NULL)
    {
        __assert_fail("temp", "Token.cpp", 0x22, "bool Token::get_token(char*, int)");
    }
    memset(temp, 0, size + 1);
    strncpy(temp, str, size);

    n_token = 0;
    char* token = strtok(temp, seps);
    while (token != NULL && n_token < MAX_TOKEN)
    {
        int nSize = strlen(token);
        if (nSize > 0x800)
        {
            nSize = 0x800;
        }
        m_token[n_token] = new char[nSize + 1];
        memset(m_token[n_token], 0, nSize + 1);
        strncpy(m_token[n_token], token, nSize);
        n_token++;
        token = strtok(NULL, seps);
    }
    return true;
}

char* Token::get_context(int idx0)
{
    if (idx0 < 0 || idx0 >= MAX_TOKEN)
    {
        __assert_fail("idx0>=0 && idx0<MAX_TOKEN", "Token.cpp", 0x42, "char* Token::get_context(int)");
    }
    return m_token[idx0];
}

int Token::get_n_token()
{
    return n_token;
}

// --- Functions defined in this compilation unit ---

// Token::get_context(int)
/* TODO: implement */

// Token::get_n_token()
/* TODO: implement */

// Token::get_token(char*, int)
/* TODO: implement */

// Token::Token()
/* TODO: implement */

// Token::Token()
/* TODO: implement */

// Token::~Token()
/* TODO: implement */

// Token::~Token()
/* TODO: implement */
