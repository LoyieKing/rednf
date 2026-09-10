Arad_InterfaceGameScript* Arad_DataManager::findGameScript(std::string name)
{
    // ORIG 0x81882ac：return m_scriptMap[name]（未命中插入空指针，返回 0）。
    return m_scriptMap[name];
