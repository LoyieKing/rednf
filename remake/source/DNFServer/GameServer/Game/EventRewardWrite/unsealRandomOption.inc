int eventReward::unsealRandomOption(const CItem* item, Inven_Item& inven)
{
    if (item == 0)
        return 0;
    // ORIG 0x80f0e40：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    if (!item->IsRandomOption())
        return 0;
    // ORIG 反汇编：第二次虚调用 vptr+0x50 = vtable 槽 20 = GetRandomOptionGrade
    //（CItem vtable @0x8c96da8：s19=IsRandomOption, s20=GetRandomOptionGrade）。
    // ORIG +0x3c..+0x42：&inven.m_random(+0x25) 在首个 getter 之前物化到 -0x1c 槽 ⇒ 具名指针局部。
    RandomOption* pOption = &inven.m_random.m_random;
    register int period = item->GetRandomOptionGrade();
    register int group = item->getItemGroupName();
    register int lvl = item->getUsableLevel();
    register int rarity = item->get_rarity();
    if (!GlobalData::g_randomOptionHandle->give_option(
            (unsigned long)inven.m_addInfo,
            (ENUM_RARITY)rarity, lvl, group, period, pOption))
    {
        LogManager::logFormat(
            1, "eventReward.cpp",
            __PRETTY_FUNCTION__, 0x240,
            "autoUnsealItem : %d", inven.m_addInfo);
    }
    return 1;
