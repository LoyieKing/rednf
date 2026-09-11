// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3c0；直读 dnfLex_.yytext。
char* DNFLexWrapper::getCustomData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}