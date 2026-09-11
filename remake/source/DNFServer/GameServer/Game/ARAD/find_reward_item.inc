// b28d-solo-ARAD verdict=IDENTICAL/AE：ORIG 0x81a3e3c 不是循环而是
// `std::find(begin, end, out)`——Ghidra C 第四实参即 out（0x10(%ebp)），
// Data::operator== 仅比 winPoint（Arad_DataManager.h:357），故 winPoint 形参未使用
//（ORIG 全程无 0xc(%ebp) 读取，原码遗留，P171 照抄）；命中后 `out = *it`
//（ORIG 0x81a3ed1 三对 dword mov = 结构整体赋值 P247）。
bool Arad_VillageAttackRewardItem::find_reward_item(int winPoint, Data& out)
{
    // ORIG 0x81a3e52：std::find<iterator,Data>(sret -0x18, begin -0x10, end -0x14, out)。
    (void)winPoint;   // ORIG 未使用该形参（比较值取自 out）
    std::vector<Data>::iterator it =
        std::find(m_rewardList.begin(), m_rewardList.end(), out);
    if (it != m_rewardList.end())
    {
        out = *it;
        return true;
    }
    return false;
}