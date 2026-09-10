std::string DNFLexWrapper::getStreamName()
{
    if (lexType_ != LEX_TYPE_NORMAL)
        return std::string("NONE");
    return dnfLex_.getStreamName();
