#ifndef SLANGFILTER_H
#define SLANGFILTER_H

#ifndef yyFlexLexer
#define yyFlexLexer __slang_filter__FlexLexer
#include "FlexLexer.h"
#endif

class SlangFilter : public __slang_filter__FlexLexer
{
public:
    SlangFilter();
    virtual ~SlangFilter();
    virtual int yylex();
    virtual void LexerOutput(const char* buf, int size);
    virtual void LexerError(const char* msg);
    bool is_clean(const char* text);
};

#endif
