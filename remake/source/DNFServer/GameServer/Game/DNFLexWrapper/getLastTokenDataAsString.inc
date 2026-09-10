std::string DNFLexWrapper::getLastTokenDataAsString()
{
    const char* pc;
    if (lexType_ == LEX_TYPE_NORMAL)
        pc = dnfLex_.yytext;
    else
    {
        pc = "";
        if (lexType_ != LEX_TYPE_COMPILED)
            goto construct;
        pc = lastStringValue_;
    }
    if (__builtin_expect(pc == 0, 0))
        goto empty;
    pc = toTString(pc);
    goto construct;
empty:
    pc = "";
construct:
    return std::string(pc);
