#ifndef TOKEN_H_
#define TOKEN_H_

#include "stdafx.h"

#define MAX_TOKEN 256

class Token
{
public:
    Token();
    ~Token();
    bool get_token(char* str, int size);
    char* get_context(int idx0);
    int get_n_token();

private:
    int n_token;
    char seps[52];
    char* m_token[MAX_TOKEN];
};

#endif // TOKEN_H_
