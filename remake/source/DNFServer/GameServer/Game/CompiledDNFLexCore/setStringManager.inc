void CompiledDNFLex::setStringManager(ScriptStringManager* pStringManager,
                                      bool allowOwner)
{
    stringManagerDelete_ = allowOwner;
    stringManager_ = pStringManager;
