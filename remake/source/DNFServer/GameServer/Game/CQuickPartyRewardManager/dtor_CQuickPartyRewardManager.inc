CQuickPartyRewardManager::~CQuickPartyRewardManager() {}  // ORIG 0x826d69c：仅基类析构

// ORIG 0x0826d6b0（报告 QuickParty__CQuickPartyRewardManager/init.md）：
// 导入 Etc/QuickParty.etc 失败 → cMyTrace 报错返回 false；成功则 new 六个奖励
// 对象写入 m_rewards 表（basic[11]=CardGold、basic[12]=HellDifficulty、
// random[0]=KillMonsterExp、random[3]=EquipDropRate、random[9]=CoinLimit、
// random[10]=ReturnTicket），最后 set_rand_buff_total_prob 并返回 true。
// b28d-20: 修前 167 insn（每个 new 后 ~9 条 EH landing pad：operator delete +
// _Unwind_Resume）；按 P216 给 6 个奖励类 ctor 声明/定义加 throw()（见 .h 同批
// 判词）后 109=109 逐条 AE（ORIG 0x0826d6b0 无任何 EH cleanup）。
