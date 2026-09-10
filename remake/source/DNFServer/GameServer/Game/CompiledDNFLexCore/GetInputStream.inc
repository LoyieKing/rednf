CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream()
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
