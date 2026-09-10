char* DNFLexWrapper::getInputStreamBuffer()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.buffer;
    char* r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetInputStream()->buffer;
    return r;
