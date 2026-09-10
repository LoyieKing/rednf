int DNFLexWrapper::getInputStreamSize()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.size;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return (int)compiledDnfLex_.GetInputStream()->size;
    return r;
