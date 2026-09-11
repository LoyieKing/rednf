// ---- IsStream（ORIG 0x8acfe80）：栈非空。----
bool CompiledDNFLex::IsStream() const
{
    return !streamStack_.empty();
}