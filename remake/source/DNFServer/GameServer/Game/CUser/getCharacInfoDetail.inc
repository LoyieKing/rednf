// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864bdba | _ZNK5CUser19getCharacInfoDetailEijP11CHARAC_DATA
bool CUser::getCharacInfoDetail(int param_1, unsigned int characNo, CHARAC_DATA* out) const
    // 2026-09-10（G3CUser-B 复验）：栈槽集 ORIG/我方 **均为 0**（本函数无任何标量局部槽，
    //   682:686 insn）；残余 4 条来自 4.4 对 `m_characList[i]` 链式 operator[] 的
    //   调用序列/寄存器形（每字段重调 ~64 次已按 ORIG 逐点展开）⇒ N 族封顶，不动。
    // b28c-L5 SIGFIX（Main 已代落 CUser.h:3121）：ORIG @0x0864bdba 全部出口只写
    //   `mov $0x1/$0x0,%eax`（P159 尾形）+ 唯一外部消费点 GetCharacInfo @0x0864bdf2
    //   `call; xor $0x1,%eax; test %al,%al; je`（P22/P168a① bool callee 直判形）。
{
    // ORIG T 0x0864bdba：op[] 每字段重调 ~64 次（4.4 O0 不 CSE call ⇒ 逐处展开
    // 同形；具名引用缓存折成单取是 gap343 主因）。字段序 = ORIG 汇编写序
    // （报告 Ghidra C 段逐项对齐；宽度 cast 均经 ORIG 指令实证 2026-09-05 CUserC8）。
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    if ((unsigned int)m_characList[param_1].m_characNo != characNo)
        return 0;
    out->m_characNo = m_characList[param_1].m_characNo;                                   // mov (%edx),%eax←(%eax)
    strncpy(out->m_name, m_characList[param_1].m_name, 0x1e);                             // strncpy 0x1e
    out->m_field22 = m_characList[param_1].m_vill;                                     // +0x22←+0x22 movb
    out->m_field24 = m_characList[param_1].m_job;                                     // +0x24←+0x26 movb(job)
    out->m_field25 = (unsigned short)m_characList[param_1].m_level;                       // +0x25←+0x27 movzwl 零扩展
    out->m_field28 = m_characList[param_1].m_exp;                                     // +0x28←+0x2b movl
    out->m_field27 = m_characList[param_1].m_growthType.m_growthType;                        // +0x27←+0x29 movb
    out->m_unkB2.m_fieldb2 = m_characList[param_1].m_stamina;                             // +0xb2←+0x75 movb
    // ORIG 0x0864bf67..0x0864bfb0：0x52 字节 _Additioal_info **结构整体赋值**，
    // 4.1 内联为「对齐 peel + rep movsl + 尾块」（无 memcpy call；报告 138-165 行）。
    out->m_add = m_characList[param_1].m_additioalInfo;                                   // +0x60←+0x88 0x52B
    out->m_field2c = m_characList[param_1].m_fatigue;
    out->m_field2e = m_characList[param_1].m_maxFatigue;
    out->m_field30 = m_characList[param_1].m_fatigueGrownUpBuff;
    out->m_field32 = m_characList[param_1].m_fatigueBattery;
    out->m_field4d = m_characList[param_1].m_lastPlayTick;                                     // +0x4d←+0x7b movl
    out->m_unkB2.m_field118 = m_characList[param_1].m_lastPlayTickPowerWar;                            // +0x118←+0x7f movl
    out->m_guildId = m_characList[param_1].m_guildId;                                     // +0x56←+0xdbe movl
    out->m_field5a = m_characList[param_1].m_guildCreateRight;                                    // +0x5a←+0xdc2 movb
    out->m_field5b = m_characList[param_1].m_premiumFatigue;
    out->m_field5d = m_characList[param_1].m_maxPremiumFatigue;
    out->m_field5f = (m_characList[param_1].m_memberkey != 0) ? 1 : 0;                     // +0x5f←(e22!=0)
    ((const CommonTime*)m_characList[param_1].m_delReqTime)->GetCreateTime(out->m_field34); // +0x39→+0x34
    out->m_unkB2.m_fieldb3 = m_characList[param_1].m_usedFatigue;                             // +0xb3←+0x4d 2B
    out->m_unkB2.m_fieldb5 = m_characList[param_1].m_tutorialFlag;                             // +0xb5←+0x51 movl
    memcpy(out->m_unkB2.m_fieldb9, m_characList[param_1].m_tradeRestrictData, 0x10);      // +0xb9←+0x55 0x10B
    out->m_unkB2.m_fieldfe = m_characList[param_1].m_resetDwordDf4;                       // +0xfe←+0xdf4
    out->m_unkB2.m_field102 = m_characList[param_1].m_resetDwordDf8;                      // +0x102←+0xdf8
    out->m_unkB2.m_fieldc9 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotal();
    out->m_unkB2.m_fieldcd =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotalBillion();
    out->m_unkB2.m_fieldd1 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldDaily();
    out->m_unkB2.m_fieldd5 = m_characList[param_1].m_chaosPoint;
    out->m_unkB2.m_fieldd9 = m_characList[param_1].m_chaosExp;
    out->m_unkB2.m_fielddd = m_characList[param_1].m_chaosModeCount;
    out->m_unkB2.m_fielde1 = m_characList[param_1].m_chaosKillCount;
    out->m_unkB2.m_fielde5 = m_characList[param_1].m_chaosDieCount;
    out->m_unkB2.m_fielde9 = m_characList[param_1].m_chaosDieTime;
    out->m_unkB2.m_fielded = m_characList[param_1].m_assaultCount;
    out->m_unkB2.m_fieldf1 = m_characList[param_1].m_chaosKillTime;
    out->m_unkB2.m_fieldf5 = m_characList[param_1].m_opencoin;                            // movb
    out->m_unkB2.m_fieldf6 = m_characList[param_1].m_luckPoint;                            // movl
    out->m_unkB2.m_fieldfa = m_characList[param_1].m_dungeonPlayCount;
    out->m_unkB2.m_field10a = m_characList[param_1].m_expertJobExp;                           // ORIG 序 10a 先于 106
    out->m_unkB2.m_field106 = m_characList[param_1].m_expertJobType;
    out->m_unkB2.m_i134 = (int)get_acc_id();                           // +0x134←get_acc_id
    out->m_unkB2.m_field10e = m_characList[param_1].m_powerWarPoint;                          // 2B
    out->m_field23 = m_characList[param_1].m_villPrev;                                     // +0x23←+0x25 movb（ORIG 尾段）
    out->m_unkB2.m_field110 = m_characList[param_1].m_powerWarAssaultCount;                          // movl
    out->m_unkB2.m_field114 = m_characList[param_1].m_powerWarAssaultVictoryCount;                          // movl
    // b28d-GrpD：ORIG 0x0864c58b..0x0864c5c1 与 0x0864c5b2..0x0864c5e8 两段**逐条同形**
    //   （各 4 条 op[] call + `mov 0x1225(%eax),%edx; mov 0x14(%ebp),%eax;
    //   mov %edx,0x114(%eax)`）；报告 Ghidra C 段亦连续两条 `param_3+0x114 = iVar1+0x1225`
    //   ⇒ 原码此处是**重复赋值**（P171 照抄 ORIG）。
    out->m_unkB2.m_field114 = m_characList[param_1].m_powerWarAssaultVictoryCount;                          // movl
    out->m_unkB2.m_field11c = (unsigned short)m_characList[param_1].m_emotion;          // movzwl 122c
    out->m_unkB2.m_field11e = m_characList[param_1].m_skillTreeIndex;
    out->m_unkB2.m_field11f = m_characList[param_1].m_exEquipSlotStat;
    out->m_unkB2.m_field120 = m_characList[param_1].m_eventCharacterGrowtype;
    out->m_unkB2.m_field121 = (unsigned char)m_characList[param_1].m_memberDungeonFlag;
    out->m_unkB2.m_field122 = m_characList[param_1].m_memberBonusFatigue;                          // 2B
    out->m_unkB2.m_b124 = (char)m_characList[param_1].m_guildSecede;
    out->m_unkB2.m_i125 = m_characList[param_1].m_lastPlayDungeonIndex;                              // movl
    out->m_unkB2.m_b129 = (char)m_characList[param_1].m_equipslotFlag;
    out->m_unkB2.m_b12a = (char)m_characList[param_1].m_channelEquipslotSwitch;
    out->m_unkB2.m_b12b = (char)m_characList[param_1].m_expandEquipslotSwitch;
    out->m_unkB2.m_b12d = (char)m_characList[param_1].m_useBringUpMaterialTotal;                        // ORIG 序 12d 先于 12c
    out->m_unkB2.m_b12c = (char)m_characList[param_1].m_bringUpRewardFlag;
    out->m_unkB2.m_b12e = m_characList[param_1].m_tagVisibleFlags.m_flag;                 // movzbl e40
    out->m_unkB2.m_b12f = (char)m_characList[param_1].m_growthPowerReward;
    out->m_unkB2.m_i130 = m_characList[param_1].m_chaosResponTime;                              // movl
    return 1;
}