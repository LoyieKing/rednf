void SlangFilter::LexerOutput(const char* buf, int size)
{
    (void)size;
    std::fprintf(stdout, buf);
