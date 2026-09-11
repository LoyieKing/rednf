// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
CompiledDNFLex::~CompiledDNFLex()
{
    CloseInputStream();
    destroy();
}