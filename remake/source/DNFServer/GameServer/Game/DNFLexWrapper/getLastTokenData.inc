char* DNFLexWrapper::getLastTokenData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
