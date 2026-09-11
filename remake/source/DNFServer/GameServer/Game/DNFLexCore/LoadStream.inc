// ---- DNFLex::LoadStream(...)（ORIG 0x8acd220）----
bool DNFLex::LoadStream(const char* dir, const char* filename, stream_data_t* s)
{
    if (loadStream == 0)
        return false;
    return loadStream(dir, filename, s);
}