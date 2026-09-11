// ORIG 0x081beab0 T get_nMaxBuyableCount（报告 get_nMaxBuyableCount.md 全文映射）：
// 对六种货币价格（money/cera/winpoint/finishpoint/guildPowerWarPoint/userPowerWarPoint）
// [b28d-29] N18b/P197③：ORIG 三目 min 形（cmp mem,%eax + jge 双出口）+ 两 dead 局部预清；4.4 折 reg-reg cmp 且丢预清  (ORIG 0x081beab0 insn 130->123)
// 逐项取「持有量 / 单价」与 0x7fffffff 取小，返回可购上限。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 34（ORIG 130 / ours 122），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
unsigned int DisPatcher_BuyItem::get_nMaxBuyableCount(CUser* user, BuyItemParam& param)

{
    // [b28d] ORIG 0x081beab6..0x081beacb：六个结果局部全为函数头声明（槽序=声明序，
    // P28/P331）：maxCount -0x24、byMoney -0x20、byCera -0x1c、byWinPoint -0x18、
    // byFinishPoint -0x14（以上无预清）、byGuildPowerWarPoint -0x10 与
    // byUserPowerWarPoint -0xc（=0 预清两条，P198① 两句式）。
    // 每块用三目 min 形（ORIG 0x081beafa `mov maxCount,%eax; cmp byX,%eax; jge L;
    // mov maxCount,%eax; jmp; L: mov byX,%eax`），非 if 形。
    // [Lane5] P209/P310 试形：局部 int 化（ORIG 比较为 `mov maxCount,%eax; cmp byX,%eax`
    // 的 mem 直比形；unsigned 局部使 4.4 双载寄存器）。
    int maxCount = 0x7fffffff;
    int byMoney;
    int byCera;
    int byWinPoint;
    int byFinishPoint;
    int byGuildPowerWarPoint = 0;
    int byUserPowerWarPoint = 0;
    if (param.m_80 != 0)
    {
        byMoney = (int)((unsigned int)user->getCurCharacMoney() / (unsigned int)param.m_80);
        maxCount = (maxCount < byMoney) ? maxCount : byMoney;
    }
    if (param.m_84 != 0)
    {
        byCera = (int)((unsigned int)user->GetCera() / (unsigned int)param.m_84);
        maxCount = (maxCount < byCera) ? maxCount : byCera;
    }
    if (param.m_88 != 0)
    {
        byWinPoint = (int)((unsigned int)user->GetWinPoint() / (unsigned int)param.m_88);
        maxCount = (maxCount < byWinPoint) ? maxCount : byWinPoint;
    }
    if (param.m_8c != 0)
    {
        byFinishPoint = (int)((unsigned int)user->GetFinishPoint() / (unsigned int)param.m_8c);
        maxCount = (maxCount < byFinishPoint) ? maxCount : byFinishPoint;
    }
    if (param.m_90 != 0)
    {
        byGuildPowerWarPoint =
            (int)((unsigned int)user->GetGuildPowerWarPoint() / (unsigned int)param.m_90);
        maxCount = (maxCount < byGuildPowerWarPoint) ? maxCount : byGuildPowerWarPoint;
    }
    if (param.m_94 != 0)
    {
        byUserPowerWarPoint =
            (int)((unsigned int)(int)(short)user->GetUserPowerWarPoint() / (unsigned int)param.m_94);
        maxCount = (maxCount < byUserPowerWarPoint) ? maxCount : byUserPowerWarPoint;
    }
    return maxCount;
}
