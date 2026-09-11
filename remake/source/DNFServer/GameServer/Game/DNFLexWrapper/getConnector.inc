// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace3f0；直读 dnfLex_.yytext（P0 表达式直读，无中间局部）。
char* DNFLexWrapper::getConnector()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}