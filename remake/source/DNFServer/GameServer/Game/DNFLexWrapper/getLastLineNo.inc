int DNFLexWrapper::getLastLineNo()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yylineno;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetOffset();
    return r;
