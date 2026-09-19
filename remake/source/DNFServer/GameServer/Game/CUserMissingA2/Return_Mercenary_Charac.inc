// ----------------------------------------------------------------------------
// ORIG T 0x0868e692：转发 UserMercenaryInfoMgr::ReturnCharac（receiver
// lea this+0x6ef6c）。
// ----------------------------------------------------------------------------
bool CUser::Return_Mercenary_Charac(unsigned int mercUid,
                                    competition_reward_result& reward)
{
    m_mercenaryMgr.ReturnCharac(this, mercUid, reward);
    // ORIG 0x0868e692 为 void 转发（leave; ret 无 eax 设置），调用点取 %al 残留——
    // 此处不写 return 以复刻 ORIG 的未定义行为。
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}