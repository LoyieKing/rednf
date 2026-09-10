void CParty::cal_hellparty_value(int dungeonDiff, int rateLevelDiff, int memberCount)
{
    if (rateLevelDiff == 0 || memberCount < 0 || dungeonDiff >= 5)
        return;
    // [b28d-solo-CPartyDepsD2] P183 去 base 缓存 + P252 具名 float 局部复现槽往返：
    // ORIG 0x085b58d0..0x085b593d 三数组值先落 -0x1c/-0x18/-0x14，base -0x10，
    // old -0xc；x87 序 `fildl ratio3; fmuls dungeonRate; flds mapRate; fld1;
    // faddp; fadds base; fadds itemRate; fmulp; fadds old` ⇒ 源形
    // `(float)ratio3 * dungeonRate * (mapRate + 1.0f + base + itemRate) + old`。
    // ORIG 偏移映射（报告 cal_hellparty_value.md，BF = this+0xb24）：
    //   +0xd58 = BF+0x234 m_hellPartyScriptRate（float）
    //   +0xd34+d*4 = BF+0x210 m_hellPartyMapRate[d]
    //   +0xd48+m*4 = BF+0x224 m_hellPartyItemRate[m]
    //   +0xd18 = BF+0x1f4 m_hellPartyRatio3（int）
    //   +0xd24+r*4 = BF+0x200 m_hellPartyDungeonRate[r]
    //   +0xd0c = BF+0x1e8 m_hellPartyRatio0（int 上限）
    //   +0xb0c = bd.m_hellPartyValueTotal
    float mapRate = bf(this).m_hellPartyMapRate[dungeonDiff];
    float dungeonRate = bf(this).m_hellPartyDungeonRate[rateLevelDiff];
    float itemRate = bf(this).m_hellPartyItemRate[memberCount];
    float baseValue = bf(this).m_hellPartyScriptRate;
    // [b28d-SweepCPartyDepsD2-2] ORIG 0x085b591e 为 `movzbl; xor$1; test%al; je`
    // ⇒ P248 档② bool 字段直证：CDungeon::m_89c 已改 bool（CDungeon.h），
    // 源形写 `!m_89c` 即复现（探针 /tmp/probe_d2.cpp fE/fF/fG；uchar 的 `!=1`
    // 与显式 `(x^1)!=0` 四形恒产 `cmp $1`）。
    if (!m_battleField.m_pDungeon->m_89c)
        baseValue = 0.0f;
    float old = bd(this).m_hellPartyValueTotal;
    // ORIG 0x085b5964 fstps 0xb0c(%eax) 直存（无 value 槽）；ROUND 段重读成员。
    bd(this).m_hellPartyValueTotal =
        (float)bf(this).m_hellPartyRatio3 * dungeonRate *
            (mapRate + 1.0f + baseValue + itemRate) +
        old;
    if ((int)bd(this).m_hellPartyValueTotal > bf(this).m_hellPartyRatio0)
        bd(this).m_hellPartyValueTotal = (float)bf(this).m_hellPartyRatio0;
    send_hellParty_value();
