void CUser::gainCurCharacFatigue(int value)
{
    // C05② 实测（2026-09-10）：`maxPrefatigue` unsigned→int 一次旋钮实测 nd 44→43
    //   （ours 76 vs ORIG 73），未翻 ORIG 的 `mov -0x18(%ebp),%eax; test %eax,%eax`
    //   段落，且该局部**被后续复用**（std::min<int> 实参）⇒ 不符 Main/D01 限定
    //   「未被后续复用、非跨调用返回值、4.4 尚未折叠」⇒ **已回退**，维持 N97/P230+N18b。
    unsigned int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    unsigned int prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    // ORIG 0x864f854 调用 std::min<int>(int const&, int const&)（外部函数调用，
    // 非三元/内联）——写显式 std::min<int> 以产出同符号调用点。
    int premiumVal = std::min<int>(prefatigue, maxPrefatigue);
    int remain = (int)premiumVal - value;
    if (remain < 0)
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        unsigned int normal = getCurCharacFatigue() & 0xffff;
        int sum = (int)normal + remain;
        setCurCharacFatigue(sum < 0 ? 0 : sum);
    }
    else
    {
        setCurCharacPremiumFatigue((unsigned short)(premiumVal - (unsigned int)value));
    }
