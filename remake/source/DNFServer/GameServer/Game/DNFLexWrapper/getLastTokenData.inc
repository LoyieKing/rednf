// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace430；P0/§4.3 具名成员直读 dnfLex_.yytext（+0x1c）
// 替代 YYText()（4.4 对弱符号 inline 恒发 call，多 add/mov/call 三条；ORIG 无此 call）。
char* DNFLexWrapper::getLastTokenData()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.yytext;
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}