// ORIG _ZN13user_creature9CCreature11GetExpLevelEi T 0x83374d2（GetExpLevel.md）：
// 成长宠（m_isGrow==1）：G_CDataManager()->get_creature_exp_level(exp)；
// 非成长宠：exp / GetGrowItemCountforLevelUp()（0 视作 25）+ 1；
// 有脚本且超 GetMaxLevel 则钳到 GetMaxLevel。
// [b28d-solo-CCreatureMgr AE] P197③ 预清 + P194 xor 物化 + P59 非成长 fall + P48 两次独立 GetMaxLevel；ORIG 0x83374d2
int CCreature::GetExpLevel(int exp)
{
    // [b28d-solo-CCreatureMgr P197③/P194/P59] 逐条照 ORIG 0x83374d8..0x833756b：
    // ①level 预清 `movl $0,-0x10`；②`xor $1;test;je <成长>` ⇒ 非成长 fall；
    // ③count 槽 -0xc（浅于 level）；④尾部两次独立 GetMaxLevel call（无具名
    // maxLevel 局部，P48）。
    int level = 0;
    if (!(m_isGrow == 1))
    {
        int count = m_script->GetGrowItemCountforLevelUp();
        if (count == 0)
            count = 0x19;
        level = exp / count + 1;
    }
    else
    {
        level = G_CDataManager()->get_creature_exp_level(exp);
    }
    if (m_script != 0)
    {
        if (m_script->GetMaxLevel() < level)
            level = m_script->GetMaxLevel();
    }
    return level;
}
