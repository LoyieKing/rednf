// [b28d-solo-CStackableItem] ORIG 0x850e9f4：place 为 UsablePlace 类型（find 实例
// `…EEEES3_ET_S9_S9_RKT0_` 的 RKT0_=UsablePlace，int 版 mangled 为 `…EEEiE`）；
// 比较为单表达式 `find(...) == m_usablePlaces.end()`（end() 求值两次：-0x18 供
// 比较、-0x10 供 find），迭代器为非 const iterator（eq 实例 `eqIPN…`）。
bool CStackableItem::verifyUsablePlace(CUser* user)
{
    if (m_usablePlaces.empty())
    {
        return 1;
    }
    StackableAction::UsablePlace place =
        static_cast<StackableAction::UsablePlace>(0);
    if (G_GameWorld()->IsPVPChannel())
    {
        place = StackableAction::USABLE_PLACE_PVP;
    }
    else
    {
        switch (user->get_state())
        {
        case 5:
            place = StackableAction::USABLE_PLACE_DUNGEON;
            break;
        case 3:
            place = StackableAction::USABLE_PLACE_VILLAGE;
            break;
        }
    }
    if (std::find(m_usablePlaces.begin(), m_usablePlaces.end(), place)
        == m_usablePlaces.end())
    {
        return 0;
    }
    if ((char)m_field4 < 0 ||
        ((char)m_field4 >= 0 &&
         ((CUserCharacInfo*)user)->getCurCharFirstGrowType() ==
             (char)m_field4))
    {
        return 1;
    }
    return 0;
}