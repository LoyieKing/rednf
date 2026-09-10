bool BlueMarbleScriptManager::initBlueMarbleInfo(const char* dir, const char* file)  // [P330] 与 PvPTypes.h 同步
{
    return m_info.importScript(dir, file);
