// ORIG _ZN13user_creature9CCreature15GainExpCreatureEiP5CUser T 0x8337bbc
// （GainExpCreature.md）：仅成长宠（m_isGrow==1）；饱食度截断 >0 才继续；
// CalculateExp 结算经验；未满级或有升级 → GainExp 通知包（析构补发）；
// 升级且可普通进化则 Evolute（失败整体返 0）；最后置 m_saveFlag=1 返回 1。
// [b28d-solo-CCreatureMgr AE] 翻正三旋钮：①notify/canEvolute 去具名 bool（P0 表达式内联）；
// ②`IsAbleEvolute` int→bool（P248 档①，ORIG 调用点 0x8337d11 `test %al,%al`）；
// ③`({ levelUp && IsAbleEvolute(user); })` 语句表达式 + 嵌套 if 复现 ORIG 物化岛 + 直跳（P226）。
// 127=127 逐条全等。ORIG 0x8337bbc
bool CCreature::GainExpCreature(int exp, CUser* user)
{
    bool levelUp = false;
    if (m_isGrow != 1)
        return false;
    int stomachValue = (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                                      m_isGrow != 0);
    if (!(stomachValue > 0))
        return false;
    CalculateExp(exp, levelUp, user);
    // P0：ORIG 0x8337c2x 的 notify 判定为直 `test %al,%al`（无槽往返）⇒ 表达式
    // 内联，禁具名 bool 局部（槽化形 cmpb 反劣）。
    if (m_level < m_script->GetMaxLevel() || levelUp)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    // 同上：canEvolute 无具名局部。
    // P226 条件向：ORIG 0x8337cfb..0x8337d23 对 `levelUp && IsAbleEvolute(user)`
    // 出 mov$1/mov$0 物化岛 + test;je（无槽）⇒ 语句表达式承载短路结果。
    if (({ levelUp && IsAbleEvolute(user); }))
    {
        if (!Evolute(user))
            return false;
    }
    m_saveFlag = 1;
    return true;
}
