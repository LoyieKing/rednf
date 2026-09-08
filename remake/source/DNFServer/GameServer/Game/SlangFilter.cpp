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
    // [b28c-lane25 判词·N18a+N7] ORIG 0x08094ef2：`mov %ebx,%eax; mov %eax,-0x24(%ebp)`
    // 两段（4.4 恒折单步，N18a 反向不可造）；虚调用 `mov (%eax),%ecx; call *%ecx`
    // vs 4.4 恒 %eax（N7 间接 call 目标寄存器）。语义等价，封顶。
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
