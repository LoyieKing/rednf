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
