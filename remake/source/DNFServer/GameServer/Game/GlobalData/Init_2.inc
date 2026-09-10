bool CDBConnector::Init(STDBConnInfo& info)
{
    // ORIG 0x080ec620：rep movsl 0x5a dword（结构整体赋值，P83）→ this+4；OpenMaster()。
    // [b28c-lane17] 返回形态 `xor $1; test; je` = if(!OpenMaster()) return false; return true;
    m_connInfo = info;
    if (!OpenMaster())
        return false;
    return true;
