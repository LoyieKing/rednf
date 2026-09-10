bool CompiledDNFLex::LoadStream(const char* dir, const char* filename,
                                DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return loadStream_(dir, filename, s);
