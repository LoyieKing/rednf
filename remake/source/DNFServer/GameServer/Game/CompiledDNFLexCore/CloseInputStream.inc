void CompiledDNFLex::CloseInputStream()
{
    while (!streamStack_.empty())
    {
        PopInputStream(true);
    }
