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
