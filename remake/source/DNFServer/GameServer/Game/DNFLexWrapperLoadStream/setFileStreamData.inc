bool DNFLexWrapper::setFileStreamData(DNFLex::stream_data_t* s, const char* path)
{
    std::ifstream* ifs;
    int size;
    const char* p = path;
    DNFLex::stream_data_t* sd = s;

    if (p == 0)
        return false;
    if (sd == 0)
        return false;

    sd->clear();
    ifs = new std::ifstream(p, std::ios::in | std::ios::binary);
    if (!ifs->is_open())
        goto fail_delete;
    if (ifs->rdstate() & std::ios::badbit)
        goto fail_delete;

    ifs->seekg(0, std::ios::end);
    size = (int)ifs->tellg();
    ifs->seekg(0, std::ios::beg);
    if (size == 0)
    {
        delete ifs;
        return setStringStreamData(sd, 0, 0);
    }
    sd->stream = ifs;
    sd->size = size;
    return true;

fail_delete:
    delete ifs;
    return false;
