// 23. CUser::DoItemUnlock(int, int) T 0x08646d82
void CUser::DoItemUnlock(int paramA, int paramB)
{
    // ORIG 0x08646d82：同 DoItemLock 内联无槽形
    ((item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2)))
        ->DoItemUnlock(this, paramA, paramB);
}