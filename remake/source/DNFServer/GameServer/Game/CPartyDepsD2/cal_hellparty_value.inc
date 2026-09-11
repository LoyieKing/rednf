// ---------------------------------------------------------------------------
// CParty::cal_hellparty_value —— ORIG T 0x085b58ac（报告 cal_hellparty_value.md）：
//   地狱值累计（上限钳制 + send_hellParty_value）。
//   +0xd0c..+0xd5b（m_padcee 未建模 hell 值表区）按报告偏移访问。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 NEAR（83=83，gap 1→0；p167 26/26 CLEAR）：
// 已修 P183 去 bf 引用缓存、P252 三数组具名 float 局部、P243 乘序、成员直存
// （fstps 0xb0c 无 value 槽）、比较反向。原「m_89c!=1 的 xor 形不可造」N 判词
// 已作废：CDungeon::m_89c uchar→bool（SweepCPartyDepsD2-2，值域 {0,1} 直证：
// STDungeonScript.cpp:1963 ScanInt((bool*)0)!=0 → CDungeon.cpp:457 拷贝）后，
// 源形 `!m_89c` 与 ORIG 0x085b5915 `movzbl; xor$1; test%al; je` 同形 ⇒
// DIFF→NEAR。残余：三处数组寻址常量拆分（ORIG `add $0x348; mov
// 0x14(%eax,%edx,4)` vs 4.4 `add $0x34c; mov 0x4(...)`），指令数相同，仅常量
// 折叠形差（N94/N4）。
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
}