CSharedServerMessageManager* CGameManager::GetSharedServerMessageManager()
{
    if (m_pSharedServerMessageMgr == 0)
    {
        // ORIG lea 0x7d8(dm)：CDataManager 内嵌 stSharedServerMessageInfo。
        // 该结构即 ServerParameterScript::m_shared770（ServerParameterScript.h +0x770，
        // ORIG C1 0x836fed4 实证），m_serverParameter 位于 CDataManager+0x68，
        // 0x68+0x770=0x7d8 与 ORIG lea 偏移一致（不缓存 dmp，对齐 ORIG 寄存器形态）。
        m_pSharedServerMessageMgr = new CSharedServerMessageManager(
            &((CDataManager*)G_CDataManager())->m_serverParameter.m_shared770);
    }
    return m_pSharedServerMessageMgr;
}