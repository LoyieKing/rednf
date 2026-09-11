const CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream() const
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}