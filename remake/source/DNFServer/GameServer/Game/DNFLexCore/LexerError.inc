void DNFLex::LexerError(const char* msg)
{
    if (lexerError != 0)
        lexerError(msg);
}