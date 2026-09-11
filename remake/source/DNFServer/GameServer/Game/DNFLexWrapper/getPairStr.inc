char* DNFLexWrapper::getPairStr()
{
    // 语义：NORMAL 用静态 buffer + yytext 成员直读；COMPILED 返回 lastStringValue_。
    static char buffer[0x100];
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        std::strncpy(buffer, dnfLex_.yytext + 1, 0x100);
        buffer[std::strlen(buffer) - 1] = 0;
        return buffer;
    }
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}