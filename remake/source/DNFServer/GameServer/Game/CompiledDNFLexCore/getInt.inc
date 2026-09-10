int CompiledDNFLex::getInt(bool* result)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d > 1 && tok != DNF_TOK_PAIR_INT)
    {
        if (result != 0)
            *result = false;
        return 0;
    }
    if (result != 0)
        *result = true;
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    return intBuffer;
