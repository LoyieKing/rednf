bool CUser::verifyStackableItemProcess(StackableAction::Type type, const CItem* item)   // [b28d-Main·P292] int→bool
{
    if (item == 0)
        return 0;
    if (!item->is_stackable())
        return 0;
    // ORIG 0x0868f60e：三处调用每次重读 0x10(%ebp) arg 槽（`mov 0x10(%ebp),%eax;
    // mov 0xc/0x8(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp)`）——具名 cast
    // 中转变量会膨胀 -0x10/-0xc 槽（技巧 36/37），cast 内联。
    if (!((CStackableItem*)item)->verifyStackableAction(type))
        return 0;
    if (!((CStackableItem*)item)->verifyUsablePlace(this))
        return 0;
    if (!((CStackableItem*)item)->verifyUsableServerType())
        return 0;
    return 1;
