// ---- SetHandler（ORIG 0x8acfdf0）----
void CompiledDNFLex::SetHandler(TLexerError lexerErrorFunc,
                                TLoadStream loadStreamFunc)
{
    lexerError_ = lexerErrorFunc;
    loadStream_ = loadStreamFunc;
}