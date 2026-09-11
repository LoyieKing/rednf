void DNFLex::LexerOutput(const char* buf, int size)
{
    if (lexerOutput != 0)
        lexerOutput(buf, size);
}