// ============================================================================
// CUser::set_growth_power_exp_reward(int)
// ORIG T 0x0868dace _ZN5CUser27set_growth_power_exp_rewardEi：
// [b28d-solo-W7_LinkTailGrowth 判词·NEAR] ORIG 0x0868dace（111/111 指令数全等，
// p167 调用点差集 14/14 CLEAR）：① `&&` 条件改 P226 语句表达式，复现 ORIG
// +0x6d..+0x8f 的短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）；
// ② Value→m_reward 的字节写改 STExpReward union 具名视图（§4.3，删裸 char* 指针视图）；
// ③ 下标比较改有符号 `idx < (int)sz`（ORIG +0xf3 cmpl/jge，无符号版产 jae=P196 隐患）。
// 残差：N7 虚调用目标寄存器 %eax↔%edx（ORIG %edx；P230 判据=4.4.6 后端可翻，
// 路由表不改，故 4.4.7 封顶）＋ 尾部对齐 nop（§6.1 豁免）＋ `m_reward.m_reward=`
// 段 4.4 RHS-first vs ORIG LHS-first（P211 档③ packed 字段侧旋钮；STExpReward.h
// 换型会波及全消费点，按边界封顶）。callee is_growth_power_exp_reward_right
// 返回类型 unsigned char→bool 由 Main 代落（P248 档①，ORIG 调用点 xor $1 实证），
// 两处 xor 已归位。
//   ev = s_event_manager->GetRepeatEvent(0x5d);
//   if (ev->IsEventing(0))：                                  // +0x09..+0xb0
//     Value value;                                            // ctor 0x8693aa0
//     if (m_seriaBlessing.get_value(level,value) &&
//         !is_growth_power_exp_reward_right())
//       写 m_expReward 字节 [+2]=value.m_fieldC、[+3]=value.m_field10、
//                          [+1]=value.m_field8                // 0x8d24f/50/4e
//     else memset(&m_reward, 0, 4);                        // 0x8d24d
//   else：                                                    // +0xe7..+0x15f
//     idx = m_growthPower.get_idx_break_away_section(level);
//     sz = m_vecC.size();                                     // dm+0x871c
//     if (idx >= 0 && idx < sz && !is_growth_power_exp_reward_right())
//       m_reward = m_vecC[idx].m_reward;          // 0x8d24d int 写
//     else memset(&m_reward, 0, 4);
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N7，gap=+0/blocks=3）→ 试=①4.4.6 后端 overlay
//   复编：四数 13/12/2/2/5 **逐项不变**（本函数仍 NEAR）⇒ 推翻「P230 换后端可治」的假设
//   （该 TU 未在路由表内，且 446 后端对 N7 间接 call 目标寄存器无效）②prett/串 CLEAR；
//   残差 3 块 = 间接 call 目标 %edx(ORIG)/%eax(我方) + 尾部 store 装载序（N7/N82）
//   ⇒ 维持 N（源码无旋钮，路由亦无效）。
void CUser::set_growth_power_exp_reward(int level)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    // [N1B 2026-09-11 负结论] ORIG 0x0868db06 段虚调用目标寄存器 = %edx
    //（`mov (%eax),%edx; …; call *%edx`），4.4 恒 %eax ⇒ 试 `(*ev).IsEventing(0)`
    // 形已实测**无效**（残差逐字不变，仍 %eax）⇒ N7 寄存器角色封顶（P230：须改
    // 路由后端，路由表不动）。
    if (ev->IsEventing(0))
    {
        stSeriaBlessingScript::Value value;               // +0x15 清零 ctor
        // ORIG +0x6d..+0x8f：`&&` 短路物化岛（mov $1/jmp/mov $0/test;je，值无槽往返）
        // ⇒ P226 扩展「语句表达式包住整个条件」（具名 bool 反产槽化+物化劣化）。
        if (({ G_CDataManager()->m_seriaBlessing.get_value(level, value) &&
               !is_growth_power_exp_reward_right(); }))
        {
            // ORIG +0x93..+0xbd：字节 store 序 +2(m_fieldC)→+3(m_field10)→+1(m_field8)，
            // 目标 char 与 RHS int 异型 ⇒ RHS-first+edx 中转（P211 档③）；用 STExpReward
            // 的 union 字节视图具名访问（§4.3，禁裸偏移指针视图）。
            m_reward.m_field5 = static_cast<char>(value.m_fieldC);    // +0x8d24f
            m_reward.m_powerUp = static_cast<char>(value.m_field10);  // +0x8d250
            m_reward.m_field4 = static_cast<char>(value.m_field8);    // +0x8d24e
        }
        else
        {
            memset(&m_reward, 0, 4);                       // +0xc2 分支
        }
    }
    else                                                      // +0xbb5 分支
    {
        // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（+0xca/+0xdf/+0x135
        // 共 3 处，加 if 分支 m_seriaBlessing 1 处 = 4 次调用）。
        const int idx = G_CDataManager()->m_growthPower.get_idx_break_away_section(level); // +0xca
        const int sz = G_CDataManager()->m_growthPower.m_vecC.size();  // +0xdf
        if (idx >= 0 && idx < sz &&                            // +0xeb cmpl/js、+0xf3 cmpl/jge（有符号）
            !is_growth_power_exp_reward_right())             // +0xfb
        {
            // [N1B 2026-09-11 负结论] ORIG 0x0868d24d 段：`mov 0x8(%ebp),%edx;
            // mov (%eax),%eax; mov %eax,0x8d24d(%edx)` = LHS(this)先算；4.4 恒
            // RHS-first。显式 `this->` 已实测**无效**（残差逐字不变）⇒ N7 族封顶。
            m_reward.m_reward = G_CDataManager()->m_growthPower.m_vecC[idx].m_reward;  // +0x135
        }
        else
        {
            memset(&m_reward, 0, 4);                      // +0x16f 分支
        }
    }
}