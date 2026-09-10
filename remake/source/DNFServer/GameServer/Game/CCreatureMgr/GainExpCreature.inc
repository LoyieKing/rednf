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
