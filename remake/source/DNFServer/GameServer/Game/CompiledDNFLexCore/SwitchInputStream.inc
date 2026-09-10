bool CompiledDNFLex::SwitchInputStream(const char* dir, const char* filename,
                                       char* buffer, unsigned int bufferSize)
{
    CloseInputStream();
    return PushInputStream(dir, filename, buffer, bufferSize);
