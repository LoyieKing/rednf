void CUser::SetPvpIndex(short idx)
{
    // ORIG 0x08669ee8：CSwitchLog("SetPvpIndex",0x2339,0,0) 记
    // "pvp@log %s,%s,room(%d)"（acc_name/charac_name/idx）后加锁写 m_pvpRoomIdx。
    // b28d-PoolCUser 重写（2026-09-09，P227/N80）：ORIG 0x085df9b2 段内 varargs
    //   （get_acc_name→esi / getCurCharacName→ebx / idx movswl→edi）**先于 ctor**
    //   求值且跨 ctor callee-saved 保活 ⇒ 原码是全表达式临时链形（非两句具名）。
    CSwitchLog("SetPvpIndex", 0x2339, 0, 0)(
        "pvp@log %s,%s,room(%d)", get_acc_name(), getCurCharacName(), (int)idx);
    lock();
    m_pvpRoomIdx = idx;
    unlock();
