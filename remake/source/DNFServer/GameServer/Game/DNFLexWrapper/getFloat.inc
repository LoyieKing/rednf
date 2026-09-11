// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace730；__strtod_internal 实参直读 dnfLex_.yytext
// （替代 YYText() call），COMPILED 返回 lastFloatValue_。
float DNFLexWrapper::getFloat()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return (float)__strtod_internal(dnfLex_.yytext, 0, 0);
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastFloatValue_;
    return 0.0f;
}