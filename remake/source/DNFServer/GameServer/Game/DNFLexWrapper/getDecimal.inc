int DNFLexWrapper::getDecimal()
{
    char* p;
    char c;
    bool neg;
    int v;
    int d;

    if (lexType_ == LEX_TYPE_NORMAL)
    {
        p = dnfLex_.yytext;
        c = *p;
        if (c == '\0')
            goto ret0;
    loop:
        if (__builtin_expect((unsigned char)(c - 0x30) <= 9, 0))
            goto digit_start;
        if (__builtin_expect(c == ' ', 0))
            goto skip_space;
        if (__builtin_expect(c == '+', 0))
            goto plus;
        if (__builtin_expect(c == '-', 0))
            goto minus;
        goto ret0;
    }
    else if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
ret0:
    return 0;
skip_space:
    p = p + 1;
    c = *p;
    if (c != '\0')
        goto loop;
    goto ret0;
minus:
    p = p + 1;
    neg = true;
    c = *p;
    goto parse;
plus:
    p = p + 1;
    neg = false;
    c = *p;
    goto parse;
digit_start:
    neg = false;
parse:
    v = 0;
    d = (int)c - 0x30;
    while ((unsigned)d <= 9)
    {
        p = p + 1;
        v = v * 10 + d;
        d = (int)*p - 0x30;
    }
    if (neg)
        return -v;
    return v;
