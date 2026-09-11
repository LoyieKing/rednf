// getRewardItem（ORIG 0x0810f694）
// grade 为 char 局部直存（ORIG mov %al,-0x11）；map 迭代器非 const。
int CAvatarDisjointRandomReward::getRewardItem(
    CItem const* citem, std::vector<std::pair<int, int> >& out)
{
    // [b28d-L14 实测] 声明序 grade→actionMng（ORIG grade -0x11、actionMng -0x12）。
    int result = 0;
    const char grade = (char)citem->get_grade();
    CEventActionMng actionMng;
    std::map<char, unsigned int>::iterator it =
        static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
            ->m_map08.find(grade);
    if (it == static_cast<AvatarDisjointRandomRewardScript*>(m_script1c)
                  ->m_map08.end())
        return 0x15;
    result = actionMng.getResultItemFromLottery(it->second, out);
    if (result != 0)
        return result;
    return 0;
}