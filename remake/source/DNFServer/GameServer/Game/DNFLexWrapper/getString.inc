char* DNFLexWrapper::getString()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.tok_str();
    char* pc = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return lastStringValue_;
    return pc;
}