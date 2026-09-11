// ---- LexerError（ORIG 0x8acfe10）----
void CompiledDNFLex::LexerError(const char* msg)
{
    if (lexerError_ != 0)
        lexerError_(msg);
}