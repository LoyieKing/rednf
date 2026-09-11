bool CAccountCargo::MoveItem(int from, int to)
{
    // [b28d-solo-CAccountCargo] ORIG 0x08289f26：`!a || !b || from == to` 短路链
    // （前两项各 `xor $1; test; jne <fail 岛>`，第三项物化 mov $1/$0 后 `je <body>`；
    // P133/P214①）。原 `bool fail = !(a && b && c)` 生栈槽 bool 物化 + 极性相反。
    if (!CheckValidSlot(from) || !CheckValidSlot(to) || from == to)
        return false;
    Inven_Item tmp;
    tmp = m_slots[from];
    m_slots[from] = m_slots[to];
    m_slots[to] = tmp;
    m_alter = 1;
    return true;
}