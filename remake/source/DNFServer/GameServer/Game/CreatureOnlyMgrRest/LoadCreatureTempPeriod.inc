bool CCreatureMgr::LoadCreatureTempPeriod(
    SIG_POSTAL_CREATURE_TEMP_PERIOD* sig)
{
    // ORIG T 0x0833d9d8：每处槽位均原地重取形参（`mov 0xc(%ebp),%eax` +
    // `mov 0x4(%eax),%eax` 读 m_tempPeriod、`mov (%eax),%eax` 读 m_uid），
    // true/false 在分支内直接置 %eax ⇒ 不设中间局部（O0 下多占栈槽会偏离）。
    if (sig->m_tempPeriod != -1)
    {
        RegistTempPeriod(sig->m_uid, sig->m_tempPeriod, 0);
        return true;
    }
    return false;
