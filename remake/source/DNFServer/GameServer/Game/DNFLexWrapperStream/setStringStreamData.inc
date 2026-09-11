// setStringStreamData（ORIG 0x8ace9b0）：用 buffer/bufferSize 构造 istrstream
// 填充 stream_data_t；bufferSize==0 时先释放 buffer 再以空串+长度 1 构造
// （size=1，buffer 槽不写）。恒返回 true。
bool DNFLexWrapper::setStringStreamData(DNFLex::stream_data_t* s, char* buffer,
                                        unsigned int bufferSize)
{
    static const char* kEmptyBuf = "";
    if (bufferSize)
    {
        std::istrstream* is = new std::istrstream(buffer, (int)bufferSize);
        s->stream = is;
        s->size = (int)bufferSize;
        s->buffer = buffer;
    }
    else
    {
        const char* e = kEmptyBuf;
        ::operator delete(buffer);
        std::istrstream* is = new std::istrstream(e, 1);
        s->stream = is;
        s->size = 1;
    }
    return true;
}