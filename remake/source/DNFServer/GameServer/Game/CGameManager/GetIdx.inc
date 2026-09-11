int CGameManager::GetIdx(CUser* user)
{
    int idx = m_userPool.GetIndex(user);
    return idx;
}