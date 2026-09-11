// ---- setStringManager（ORIG 0x8ad0050）----
void CompiledDNFLex::setStringManager(ScriptStringManager* pStringManager,
                                      bool allowOwner)
{
    stringManagerDelete_ = allowOwner;
    stringManager_ = pStringManager;
}