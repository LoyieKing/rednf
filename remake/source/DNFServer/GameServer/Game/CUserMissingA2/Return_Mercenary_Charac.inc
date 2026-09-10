bool CUser::Return_Mercenary_Charac(unsigned int mercUid,
                                    competition_reward_result& reward)
{
    m_mercenaryMgr.ReturnCharac(this, mercUid, reward);
    // ORIG 0x0868e692 为 void 转发（leave; ret 无 eax 设置），调用点取 %al 残留——
    // 此处不写 return 以复刻 ORIG 的未定义行为。
