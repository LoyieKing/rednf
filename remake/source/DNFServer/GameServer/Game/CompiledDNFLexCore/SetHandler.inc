void CompiledDNFLex::SetHandler(TLexerError lexerErrorFunc,
                                TLoadStream loadStreamFunc)
{
    lexerError_ = lexerErrorFunc;
    loadStream_ = loadStreamFunc;
