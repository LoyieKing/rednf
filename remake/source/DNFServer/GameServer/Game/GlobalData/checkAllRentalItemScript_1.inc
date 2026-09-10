// [G7-sub 2026-09-11 判词] RentalMachineDimension::checkAllRentalItemScript
// （ORIG T 0x0826f022）：私有 .o 实测 ORIG **86 / 我方 86（gap 0）**，逐条规范化后
// **零形差**（仅 call 目标文本：报告侧 Ghidra 标签 vs 我方真实重定位名）。
// 注：本报告早前登记的 282/286 属**同名 PcRoom 版**（0x082708cc，在
// checkAllRentalItemScript.inc，非本件）；本件（Dimension 版）实测全等。
// 结论：**封顶，无改动**。
bool RentalMachineDimension::checkAllRentalItemScript()
{
    // ORIG 0x0826f028/0x0826f086/0x0826f0b9/0x0826f114：两循环的 begin()/end()
    // 各自独立重调 G_CDataManager()（P157 逐字段重读 + P48 无 CSE，禁 dm 缓存）；
    // `(*it).second` 代 `it->second`（P167：ORIG 发 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_mapC.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_mapC.end(); ++it)
    {
        // [b28d-solo-GlobalData] P0：ORIG 0x0826f05x `add $0x4,%eax; mov %eax,-0x10;
        // mov -0x10,%eax` 槽往返 ⇒ 原码有具名引用局部再传参（直接传表达式少 2 条）。
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    for (std::map<std::pair<int, int>,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_map24.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_map24.end(); ++it)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    return true;
