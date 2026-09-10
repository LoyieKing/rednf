bool DNFLexWrapper::LoadStream(const char* dir, const char* filename,
                               DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return reinterpret_cast<bool (*)(const char*, const char*, DNFLex::stream_data_t*)>(
        loadStream_)(dir, filename, s);
