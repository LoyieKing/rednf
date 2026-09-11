// ============================================================================
// eventReward::unsealRandomOption —— ORIG T 0x080f0e40
// ============================================================================
bool eventReward::unsealRandomOption(const CItem* item, Inven_Item& inven)
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
    // 负结论（4.4 实参 temp 分配族，N 类；三次实测均劣于本形）：
    //  ORIG +0x57..+0x71 四值保活 edi/esi/ebx/edx（0x080f0e97/0x0ea4/0x0eb1/0x0ebe），
    //  +0x8b `mov &g_randomOptionHandle,%eax; mov %eax,-0x2c` ⇒ -0x2c 专存 handle，
    //  +0x71 `mov -0x1c(%ebp),%eax; mov %eax,0x18(%esp)` pOption 实参段直读。
    //  全内联 4 getter（指针形/引用形 0x080f0e7c–0x0f31）与「三局部+末位内联」
    //  均产 80 条：4.4 必为第 7 实参 pOption 另发 temp（-0x2c 存 pOption、handle 退
    //  -0x3c），恒多 2 条 mov ⇒ 不可由源码形态消除，78=78 为本件封顶。
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
}