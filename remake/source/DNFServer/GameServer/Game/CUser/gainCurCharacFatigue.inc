// ORIG T 0x0864f7fe sz0xeb：扣除 1 点疲劳（先消耗高级会员疲劳额度）。
//   min(premiumFatigue, maxPremiumFatigue) → 不足时回落到普通疲劳。
// b28d-lane5 N封顶: N97/P230 cmov; N82/P112 槽号分配 | ORIG 0x0864f7fe | _ZN5CUser20gainCurCharacFatigueEi
void CUser::gainCurCharacFatigue(int value)
{
    // b28d-G3C 2026-09-10（P211 类型同型化 + P183/P59 形态）：
    // ① ORIG 0x0864f847/0x0864f84e `lea -0x1c(%ebp),%eax / lea -0x18(%ebp),%eax`
    //    ——两个具名槽的**地址直接**喂 std::min<int>(int const&, int const&) ⇒
    //    原码 maxPrefatigue/prefatigue 为 **int**。我方 unsigned 形须为 const int&
    //    造两个临时（多 -0x28/-0x24 槽 + 2 条 `mov %eax,-0x2c/-0x28` 拷贝）。
    // ② ORIG 0x0864f873 `sub %eax,-0x14(%ebp)`（mem 目标）⇒ 原码 `premiumVal -= value;`
    //    （我方原写 `premiumVal - (unsigned int)value` 产 16 位 `sub %ax,%cx`）。
    // ③ ORIG 0x0864f8ba `mov -0x10(%ebp),%eax; mov -0xc(%ebp),%edx;
    //    lea (%edx,%eax,1),%eax; test %eax,%eax; jns` ⇒ normal 侧**无 sum 具名槽**、
    //    if/else 两分支（我方原三目形产 `sar $0x1f; not; and` 位运算夹取 + 独有槽）。
    // 2026-09-11 L9GrpF（P-L11d/P212 声明序）：ORIG 0x0864f7fe 帧内
    //   `movl $0x0,-0x1c(%ebp)`（maxPrefatigue@-0x1c、prefatigue@-0x18），我方对调。
    //   4.4 -O0 槽向 = 先声明→浅槽 ⇒ 把 prefatigue 声明提前（初始化语句仍留在原处）。
    //   探针 /tmp/L9/F/pf3.cpp f0(现形)/f1(本形)：f1 与 ORIG 逐条同形（movl $0,-0x1c /
    //   mov %eax,-0x1c / mov %eax,-0x18 / lea -0x1c→4(%esp) / lea -0x18→(%esp)）。
    int prefatigue;
    int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    // ORIG 0x864f854 调用 std::min<int>(int const&, int const&)（外部函数调用，
    // 非三元/内联）——写显式 std::min<int> 以产出同符号调用点。
    int premiumVal = std::min<int>(prefatigue, maxPrefatigue);
    int remain = (int)premiumVal - value;
    // b28d-GrpD：ORIG 0x0864f871 `js 0x864f890` ⇒ 原码条件为 `remain >= 0`（正块
    //   fall-through、负块物理后置）。探针 /tmp/CuD/probe/p5.cpp f1/f2 实证：
    //   `>= 0` 产 js+正块在前，`< 0` 产 jns+负块在前 ⇒ 按 ORIG jcc 方向与块序改写。
    if (remain >= 0)
    {
        premiumVal -= value;
        setCurCharacPremiumFatigue((unsigned short)premiumVal);
    }
    else
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        int normal = getCurCharacFatigue() & 0xffff;
        if (normal + remain < 0)
            normal = 0;
        else
            normal = normal + remain;
        setCurCharacFatigue(normal);
    }
}