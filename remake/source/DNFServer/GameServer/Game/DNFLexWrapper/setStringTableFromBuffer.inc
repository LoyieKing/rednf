bool DNFLexWrapper::setStringTableFromBuffer(const unsigned char* buffer, int size)
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromBuffer(buffer, size);
