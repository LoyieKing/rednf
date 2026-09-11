// ---- GetInputStream（ORIG 0x8acfea0）：栈空返回 0，否则栈顶流。----
CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream()
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}