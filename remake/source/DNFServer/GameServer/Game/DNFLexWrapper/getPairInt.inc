int DNFLexWrapper::getPairInt()
{
    // 语义：NORMAL 用 yytext 成员直读（免 YYText() call），strncpy 0x100 + strlen 截尾 + strtol。
    if (lexType_ != LEX_TYPE_NORMAL)
    {
        int r = 0;
        if (lexType_ == LEX_TYPE_COMPILED)
            r = lastIntValue_;
        return r;
    }
    char buffer[0x100];
    std::strncpy(buffer, dnfLex_.yytext + 1, 0x100);
    buffer[std::strlen(buffer) - 1] = 0;
    return (int)__strtol_internal(buffer, 0, 10, 0);
}