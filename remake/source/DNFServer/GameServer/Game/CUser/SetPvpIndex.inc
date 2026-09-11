// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x085df9b2 | _ZN5CUser11SetPvpIndexEs
void CUser::SetPvpIndex(short idx)
{
    // ORIG 0x08669ee8：CSwitchLog("SetPvpIndex",0x2339,0,0) 记
    // "pvp@log %s,%s,room(%d)"（acc_name/charac_name/idx）后加锁写 m_pvpRoomIdx。
    // b28d-PoolCUser 重写（2026-09-09，P227/N80）：ORIG 0x085df9b2 段内 varargs
    //   （get_acc_name→esi / getCurCharacName→ebx / idx movswl→edi）**先于 ctor**
    //   求值且跨 ctor callee-saved 保活 ⇒ 原码是全表达式临时链形（非两句具名）。
    // [CUserLane 2026-09-11] P139：ORIG 有 r 段符号 _ZZN5CUser11SetPvpIndexEsE12__FUNCTION__
    //   （0x08cc2628/0x08cfd425，size 0xc）；手写串只生匿名 .rodata ⇒ 改 __FUNCTION__ 补符号。
    CSwitchLog(__FUNCTION__, 0x2339, 0, 0)(
        "pvp@log %s,%s,room(%d)", get_acc_name(), getCurCharacName(), (int)idx);
    lock();
    m_pvpRoomIdx = idx;
    unlock();
}