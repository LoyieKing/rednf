// ---- SwitchInputStream（ORIG 0x8ad0f40）：Close 后尾调 Push。----
bool CompiledDNFLex::SwitchInputStream(const char* dir, const char* filename,
                                       char* buffer, unsigned int bufferSize)
{
    CloseInputStream();
    return PushInputStream(dir, filename, buffer, bufferSize);
}