// ---- AvatarCoin::SaveToDB ORIG @ 0x081800d6 ----
bool AvatarCoin::SaveToDB(CUser* user)
{
    // P0：ORIG Acquire 返回值 `mov %eax,0x4(%esp)` 直传 ctor 实参，无具名 stream 局部。
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire(
        "localjapan/Arad_AvatarRoulette.cpp", 0x184), true);
    // [b23判词·AE假阳性] 69/69 归一化逐条相等（含 guard@-0x14 / sig@-0xc 槽位、
    // 两次 operator* + 一次 operator-> + GetInBuffer<SIG_SAVE_INVENTORY> 的双 call 形、
    // `movb $0x1,0x8(%eax)` 保存标志、put(2) 与清理岛的 %ebx/%esi 交换序）；残余 diff
    // 全为未 relink .o 的 call 重定位占位 + s_big_stream_pool/s_msgq_mgr/.rodata 文件名
    // 字符串显示 0x0（§6.3 假阳性），relink 后转 IDENTICAL_AE。
    guard.operator*()->operator<<(0x23);              // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)user->GetUID());

    // ORIG 经 guard.operator->()（0x080c8c4e）再调 GetInBuffer，两层 call。
    SIG_SAVE_INVENTORY* sig = guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();
    user->_GetSaveInvenBase(sig);
    sig->m_saveFlag = 1;  // ORIG local_10[8]=1（仅 AvatarCoin 保存标志）

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}