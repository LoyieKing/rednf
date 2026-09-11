// ORIG T 0x08100c92 (0x3f)：mgr->Finalize → ~CIPGMgr → delete → 清指针。
void CIPGHelper::Finalize()
{
    m_mgr->Finalize();
    // ORIG 0x08100c92：单次 `mov 0x4,%ebx; test %ebx; je` 守卫。外层 if(m_mgr!=0)
    // + delete 造成双重判空（+4 条）。delete 自带空守卫（P141），去外层 if。
    delete m_mgr;
    m_mgr = 0;
}
