// ---- CloseInputStream（ORIG 0x8ad0e90）：反复弹出至空。----
void CompiledDNFLex::CloseInputStream()
{
    while (!streamStack_.empty())
    {
        PopInputStream(true);
    }
}