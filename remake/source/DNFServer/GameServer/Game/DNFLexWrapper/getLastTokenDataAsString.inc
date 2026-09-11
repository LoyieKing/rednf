// [GiBodyLex] AE：ORIG 0x08ace4d0（33 insn）的 `return std::string(pc);` 只发 toTString +
// _ZNSsC1EPKcRKSaIcE + _Unwind_Resume 三个 call。此前本件因 ScanAll×2 的
// `__attribute__((optimize("O0")))` 触发 4.4 TU 级内联抑制而多发 _ZNSaIcEC1Ev×1 +
// _ZNSaIcED1Ev×2（p167 EXTRA=3）；ScanAll×2 已按 R3 迁至 DNFLexWrapperHelpers.cpp
// （ORIG 中与 ScanStrOrInt/ScanAnyData 同属 0x088b 段 -O0 TU），本 TU 内联抑制随之消除。
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
}