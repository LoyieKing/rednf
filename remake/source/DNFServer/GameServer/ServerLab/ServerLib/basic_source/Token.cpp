#include <assert.h>
#include <string.h>

#include "Token.h"

namespace nsl {

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
}

bool Token::get_token(char* str, int size)
{
    if (size <= 0)
    {
        return false;
    }

    char* temp = new char[size + 1];
    // ORIG __LINE__ 实测 0x20=32（point/auction 同源一致）
#line 32
    assert(temp);
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
    delete[] temp;
    return true;
}

char* Token::get_context(int idx0)
{
    // ORIG __LINE__ 实测 0x42=66（point/auction 同源一致）
#line 66
    assert(idx0>=0 && idx0<MAX_TOKEN);
    return m_token[idx0];
}

void Token::free_token(int idx0)
{
    // ORIG __LINE__ 实测 0x48=72（point/auction 同源一致）
#line 72
    assert(idx0>=0 && idx0<MAX_TOKEN);
    if (m_token[idx0] != NULL)
    {
        operator delete[](m_token[idx0]);
    }
    n_token = 0;
}

int Token::get_n_token()
{
    return n_token;
}

} // namespace nsl
