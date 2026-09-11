// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace490；NORMAL 分支直读 dnfLex_.yylineno（+0x24）
// 替代 lineno() call；COMPILED 尾调 GetOffset 与 ORIG 同形。
int DNFLexWrapper::getLastLineNo()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yylineno;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetOffset();
    return r;
}