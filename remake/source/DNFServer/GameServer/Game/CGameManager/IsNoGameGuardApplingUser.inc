bool CGameManager::IsNoGameGuardApplingUser(std::string& name, char* addr)
{
    // ORIG 0x82a3544：单表达式比较 → 实参右先求值：end() 临时占深槽 -0x10、
    // find(name) 临时占 -0xc（具名两变量写法槽序相反）
    if (m_noGameGuard.find(name) == m_noGameGuard.end())
        return false;
    return true;
