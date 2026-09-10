int DisPatcher_DeleteItem::condition_check(CUser* user, MSG_BASE& msg, INVEN_TYPE& type, int idx)

{
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    if (type == INVEN_TYPE_INVEN && req.m_position[idx] <= 9)
    {
        return 0x17;   // 普通背包槽位 <=9 受保护
    }
    // ORIG +0x3f..+0x5c：position[idx] 短整表达式作普通实参直传（P200，
    // `movzwl field,%eax; movswl %ax,%ebx` 跨 GetInvenType 存活于 %ebx）。
    // 禁具名中间局部/register asm（退化 cwtl+mov 两步）；直接内联为实参。
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_itemSpace),
            (int)req.m_position[idx]))
    {
        return 0xd5;   // 物品锁定
    }
    return 0;
}
