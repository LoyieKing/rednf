bool DNFLexWrapper::setStringTableFromFile(const char* fileName)
{
    ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    return mgr->importFromFile(fileName);
