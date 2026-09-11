// ORIG T _ZN27CInGameAdvertisementManager16InitADExposeInfoEv @0x080fd122
// （报告 InitADExposeInfo.md）：遍历 multimap delete 各 info → clear(this)。
// ORIG 循环体将 it->second 存入命名局部(-0xc)再 delete（P28 槽位序）。
void CInGameAdvertisementManager::InitADExposeInfo()
{
    for (std::multimap<short, CAdvertisementExposeInfo*>::iterator it =
             m_exposeInfo.begin();
         it != m_exposeInfo.end(); ++it)
    {
        CAdvertisementExposeInfo* info = it->second;
        delete info;
    }
    m_exposeInfo.clear();
}