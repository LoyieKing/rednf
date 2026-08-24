#include "SlangFilter.h"

#include <cstdio>
#include <cstring>
#include <strstream>

SlangFilter::SlangFilter()
    : __slang_filter__FlexLexer(0, 0)
{
}

SlangFilter::~SlangFilter()
{
}

void SlangFilter::LexerOutput(const char* buf, int size)
{
    (void)size;
    std::fprintf(stdout, buf);
}

void SlangFilter::LexerError(const char* msg)
{
    std::fprintf(stderr, msg);
}

bool SlangFilter::is_clean(const char* param_1)
{
    int iVar3;
    size_t sVar2;
    std::istrstream* this_00;
    char* local_24;
    char* local_20;

    if (param_1 == 0)
        return 0;
    sVar2 = std::strlen(param_1);
    if (sVar2 == 0)
        return 0;

    this_00 = new std::istrstream(param_1, (int)sVar2);
    switch_streams(this_00, 0);
    local_24 = (char*)param_1;
    do
    {
        iVar3 = yylex();
        if (iVar3 == 1)
        {
            local_20 = (char*)param_1;
            local_24 = (char*)std::strstr(local_24, YYText());
            if (local_24 == 0)
                goto next_tok;
            for (; local_20 <= local_24; local_20 = local_20 + 1)
            {
                if (local_20 == local_24)
                    return 0;
            }
            local_24 = local_20;
        }
    next_tok:
        ;
    } while (iVar3 != 2);
    if (this_00 != 0)
        (*(void (**)(std::istrstream*))(*(int*)this_00 + 4))(this_00);
    return 1;
}
