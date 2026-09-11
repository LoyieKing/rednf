void CDataManager::SetScriptPacks(const char* path)
{
    if (path == 0)
    {
        m_scriptPacks = "Script.pvf";
    }
    else
    {
        m_scriptPacks = path;
    }
}