void CUser::DoItemLock(int paramA, int paramB)
{
    // ORIG 0x08646d4a：无 lock 槽（帧 0x18），首 call 结果 %eax 跨实参构造直落 (%esp)
    ((item_lock::CItemLock*)GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(2)))
        ->DoItemLock(this, paramA, paramB);
