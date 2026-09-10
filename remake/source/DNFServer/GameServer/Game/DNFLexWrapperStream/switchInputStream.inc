bool DNFLexWrapper::switchInputStream(const char* dir, const char* filename,
                                      DNFLex::stream_data_t* s)
{
    char errbuf[0x400];
    DNFLex::stream_data_t local;
    short magic;

    if (s == 0)
    {
        s = &local;
        s->clear();
        if (!LoadStream(dir, filename, s))
        {
            if (lexerError_ == 0)
                return false;
            ss_sprintf(errbuf, "%s/%s Read failed!",
                       toMbcs(dir), toMbcs(filename));
            reinterpret_cast<void (*)(const char*)>(lexerError_)(errbuf);
            return false;
        }
    }
    if (1 < (unsigned)s->size)
    {
        reinterpret_cast<std::istream*>(s->stream)->read(
            reinterpret_cast<char*>(&magic), 2);
        reinterpret_cast<std::istream*>(s->stream)->seekg(-2, std::ios::cur);
        if (magic == static_cast<short>(0xd0b0))
        {
            char* pc = s->buffer;
            lexType_ = LEX_TYPE_COMPILED;
            if (pc == 0)
            {
                pc = new char[s->size];
                reinterpret_cast<std::istream*>(s->stream)->read(pc, s->size);
                return compiledDnfLex_.SwitchInputStream(
                    dir, filename, pc, static_cast<unsigned int>(s->size));
            }
            bool ret = compiledDnfLex_.SwitchInputStream(
                dir, filename, pc, static_cast<unsigned int>(s->size));
            if (s->stream != 0)
            {
                void (**vtable)(void*) =
                    *reinterpret_cast<void (***)(void*)>(s->stream);
                vtable[1](s->stream);
            }
            return ret;
        }
    }
    lexType_ = LEX_TYPE_NORMAL;
    return dnfLex_.SwitchInputStream(dir, filename, s);
