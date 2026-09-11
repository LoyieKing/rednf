// ORIG _ZN13user_creature12CCreatureMgr22LoadCreatureTempPeriodEP31SIG_POSTAL_...
// T 0x0833d9d8（LoadCreatureTempPeriod.md）：+4 非 -1 时 RegistTempPeriod(uid,t,0)；
// 返回 t!=-1。（结构 SIG_POSTAL_CREATURE_TEMP_PERIOD 由 decl 收编，仅证 +0/+4）
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
}