float DNFLexWrapper::getFloat()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return (float)__strtod_internal(dnfLex_.yytext, 0, 0);
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastFloatValue_;
    return 0.0f;
