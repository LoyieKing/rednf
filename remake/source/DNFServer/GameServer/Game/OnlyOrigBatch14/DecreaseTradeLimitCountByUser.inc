// ===== _Z29DecreaseTradeLimitCountByUserR5CUserii —— ORIG 0x0812017d =====
// curCharac 缺失 → false；GetInvenRef(space, slot) 缺失 → false；
// 否则委托 DecreaseTradeLimitCount(Inven_Item&)。
bool DecreaseTradeLimitCountByUser(CUser& user, int space, int slot)
{
    if (user.getCurCharacR() == 0)
    {
        return false;
    }
    // ORIG getCurCharacInvenW 结果寄存器直传（无槽，P0 内联右值）。
    Inven_Item* item = user.getCurCharacInvenW()->GetInvenRef(space, slot);
    if (item == 0)
    {
        return false;
    }
    return DecreaseTradeLimitCount(*item);
}
