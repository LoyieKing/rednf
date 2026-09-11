// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace780；sscanf 实参直读 dnfLex_.yytext + 2
// （替代 YYText() call），COMPILED 返回 lastIntValue_。
int DNFLexWrapper::getHexDecimal()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        int v = 0;
        std::sscanf(dnfLex_.yytext + 2, "%x", &v);
        return v;
    }
    int r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastIntValue_;
    return r;
}