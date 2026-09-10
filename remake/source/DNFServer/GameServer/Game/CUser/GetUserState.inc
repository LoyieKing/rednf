int CUser::GetUserState()
{
    // b28d-PoolCUser 重写（2026-09-09，P157/P168 逐使用点重载）：ORIG 0x082f0e70
    //   每个条件均 `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax; cmp $K,%eax` 全量
    //   重读（Ghidra/asm 同址 8 次 = P48 源码两次独立读），**无 state 槽**（P0）
    //   ⇒ 删具名缓存局部。|| 短路对（5/10、6/7）je 落前置共享岛 + 第二条件
    //   jne 跳过 = 4.4 `a==x || a==y` 自然形。
    // b28d-PoolCUser 二修：ORIG `cmp $3; jg +0x24` 正向落主链、假路（<=3）跳**前置**
    //   assault 岛 + `jmp` 共享尾（P222 判据——早返假路前置岛形）⇒ 改否定早返写法。
    if (m_chState <= 3)
        return CUserCharacInfo::GetUserStateByAssault();
    if (m_chState == 5 || m_chState == 10)
        return 1;
    if (m_chState == 8)
        return 3;
    if (m_chState == 6 || m_chState == 7)
        return 2;
    if (m_chState == 12)
        return 1;
    if (m_chState == 11)
        return 7;
    if (m_chState == 12)
        return 1;
    return 0;
