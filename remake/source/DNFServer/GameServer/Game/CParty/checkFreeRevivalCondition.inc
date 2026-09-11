// ORIG 0x085be39a（_ZN6CParty25checkFreeRevivalConditionEPK8CDungeon）：
// 1) m_battleField.m_bloodState==1 → true；2) dungeon 为空时按战场 dungeon_index 找回；
// 3) CDungeon::checkFreeRevivalCondition(diff) 命中 → true；
// 4) 否则需维度副本（get_dimension_possible>0）且重复事件槽 0x5e 在进行中，
//    再按 diff 查 CDimensionActivationEvent::checkFreeRevivalCondition。
bool CParty::checkFreeRevivalCondition(CDungeon const* dungeon)
{
    if (m_battleField.m_bloodState == 1)
        return true;

    // ORIG 0x085be3e5：fallback 直接重写形参槽（mov %eax,0xc(%ebp)），无独立 dg 局部。
    if (dungeon == 0)
    {
        int idx = reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index();
        dungeon = G_CDataManager()->find_dungeon(idx);
        if (dungeon == 0)
            return false;
    }

    if (dungeon->checkFreeRevivalCondition(
            reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff()))
        return true;

    // ORIG 0x085be430：test %al;setle = 有符号字节比较（同 CPartyDepsB2 (signed char) 先例）。
    if ((char)dungeon->get_dimension_possible() <= 0)
        return false;

    // b28d-49：ORIG 085be49a `mov -0x10(%ebp),%eax; mov %eax,(%esp)` = receiver 在实参
    // 之后载入 eax（非 cast 表达式形，P277 探针 k9/ka/k3）⇒ event 局部本身声明为派生指针，
    // 直接调用 CDimensionActivationEvent::checkFreeRevivalCondition（非虚，直接 call）。
    // IsEventing 经派生指针仍是虚调用（ORIG 085be476 `call *%edx`）。
    CDimensionActivationEvent* event =
        static_cast<CDimensionActivationEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x5e));
    if (event != 0)
    {
        if (!event->IsEventing(0))
            return false;
        return event->checkFreeRevivalCondition(
            reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff());
    }
    return false;
}