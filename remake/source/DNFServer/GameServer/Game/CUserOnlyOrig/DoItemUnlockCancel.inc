// 24. CUser::DoItemUnlockCancel(int, int) T 0x08646dba
void CUser::DoItemUnlockCancel(int paramA, int paramB)
{
    // ORIG 0x08646dba：同 DoItemLock 内联无槽形
    ((item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2)))
        ->DoItemUnlockCancel(this, paramA, paramB);
}