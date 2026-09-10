bool DNFLexWrapper::switchChannelInputStream(const char* filename,
                                             DNFLex::stream_data_t* s)
{
    DNFLex::stream_data_t local;
    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!setFileStreamData(s, filename))
            return false;
    }
    lexType_ = LEX_TYPE_NORMAL;
    return dnfLex_.SwitchInputStream("", filename, s);
