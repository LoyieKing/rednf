// ORIG 0x085577ac (W)：id==0 → false；否则在 loadLack 列表中查找
bool CMailBox::FindPackageLoadLack(unsigned int id)
{
    if (id == 0)
        return 0;
    return std::find(m_loadLackList.begin(), m_loadLackList.end(), id)
               != m_loadLackList.end();
}