bool CDBConnector::OpenMaster()
{
    // ORIG 0x080ec67c：Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, this+4)
    // 结果取反返回。ORIG 0x080ec6aa：xor+test 后 fall-through 块为 mov $0、
    // je 目标块为 mov $1（技巧 17：fall-through=源码首分支）——
    // `if (!Open()) return false; return true;`（与 ORIG 机器码值域一致）。
    if (!Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, m_connInfo))
        return false;
    return true;
