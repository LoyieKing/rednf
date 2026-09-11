bool GameWorld::DeleteUserInIDMap(const char* name)
{
    if (name == NULL)
    {
        return false;
    }
    if (!(strlen(name) < 0x1f))
    {
        return 0;
    }
    {
        std::string strName(name);
        std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
        // b28d-lane11：P198② 去具名 bool（ORIG +0x0dc `test %al,%al; je` 寄存器
        // 直测，具名 found 反落槽并撑帧 0x20→0x30）。
        if (it == m_idMap.end())
        {
            return false;
        }
        m_idMap.erase(it);
        return true;
    }
    return false;
}