int PvP_MissionSystem::sort_list()
{
    // b28d-61 [P175/P225，ORIG 0x085e20d4 `call operator[](i)` + `cmp -0x10(%ebp),
    // %eax; seta %al`]：索引循环（size() 左操作数 + seta 四段物化），非迭代器。
    // P159：ORIG 尾 `mov $0x0,%eax` ⇒ 返回 int（非 void）。
    // P0：ORIG 把 operator[] 结果写 -0xc 槽再回读 ⇒ 原码有具名引用局部。
    unsigned int i = 0;
    while (m_kindMissionList.size() > i)
    {
        std::list<CMission*>& l = m_kindMissionList[i];
        l.sort(MissionLessThan());
        i++;
    }
    return 0;
