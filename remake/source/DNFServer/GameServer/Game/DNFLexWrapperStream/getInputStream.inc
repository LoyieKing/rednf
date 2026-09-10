void* DNFLexWrapper::getInputStream()
{
    void* r = 0;
    if (lexType_ != LEX_TYPE_NORMAL)
        return r;
    return dnfLex_.GetInputStream()->s.stream;
