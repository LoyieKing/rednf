// ORIG _ZN13user_creature9CCreature12CalculateExpEiRbP5CUser T 0x83377b6
// （CalculateExp.md；除数 .rodata 0x8c2f450 = 100.0f 实证）：
// gained = exp*(GetAddExp(0x17)+GetAddExp(0x18)+GetAddExp(0x19))/100 截断取整；
// m_exp += gained + exp；新等级 = GetExpLevel(m_exp)，高于当前则置 levelUp。
void CCreature::CalculateExp(int exp, bool& levelUp, CUser* user)
{
    // ORIG 0x83377bc/0x83377c4：两个 float 槽 entry 各自两段物化清零
    // （-0x18 先声明=截断缓存、-0x14 后声明=累加器，技巧 43/21）。
    float scaled = 0.0f;
    float addRate = 0.0f;
    addRate += GetAddExp(0x17, user);
    addRate += GetAddExp(0x18, user);
    addRate += GetAddExp(0x19, user);
    // ORIG 0x8337840/0x8337843：`fstps -0x18; flds -0x18`——乘除结果先落具名
    // float 变量、再从其 reload 做截断（4.4 表达式内联会省掉这对）。
    scaled = (float)exp * addRate / 100.0f;
    int gain = (int)scaled + exp;                       // ORIG fistpl 截断+加 exp
    m_exp += gain;
    int newLevel = GetExpLevel(m_exp);
    if (m_level < newLevel)
        levelUp = true;
    m_level = newLevel;
}
