// switchChannelInputStream（ORIG 0x8acf060）：s==0 时先 clear 局部
// stream_data_t 并 setFileStreamData；置 lexType_=NORMAL 后转发
// DNFLex::SwitchInputStream("", filename, s)。
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
}