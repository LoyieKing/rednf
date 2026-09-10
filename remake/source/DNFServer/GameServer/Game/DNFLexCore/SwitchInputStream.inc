bool DNFLex::SwitchInputStream(const char* dir, const char* filename, stream_data_t* s)
{
    CloseInputStream();
    return PushInputStream(dir, filename, s);
