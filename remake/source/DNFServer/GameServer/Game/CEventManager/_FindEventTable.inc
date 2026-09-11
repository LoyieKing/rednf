int _FindEventTable(int id, const char* name, bool& dup)
{
    // b28d-solo-CEventManager: [AE] P0/P183——ORIG 0x0811a15d 无 tblName 具名局部、
    // strlen(name) 无条件调用（无 name/tblName 判空守卫），逐次重读 events[i].name；
    // P222 正向包裹 + 单出口（dup 块 fall-through 到 return i，ORIG 0x0811a1c1）
    // ⇒ 43=43；逐条归一化仅剩 events 绝对寻址/重定位伪差（N102/P264）。
    for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
    {
        if (events[i].id == id)
        {
            if (strncmp(events[i].name, name, strlen(name)) != 0)
                dup = true;
            return i;   // ORIG 0x0811a1c1 单出口：dup 块 fall-through 到 return i
        }
    }
    return -1;
}