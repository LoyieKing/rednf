bool CCreatureMgr::UseFeed()
{
    // ORIG 0x8339dca/0x8339df8/0x8339e98 三失败路径共享尾 + 返回值 %ebx 汇聚：
    // 源码形为单个具名 char ok 复用（Ghidra cVar2 两次赋值 + `== 1` 比较），
    // 尾部 `return ok == 1;`（4.1 出 `xor $1;test;je` 与 `mov %ebx,%eax` 汇聚）。
    if (m_equippedCreature != 0)
    {
        int stomachValue = 0;
        if (m_equippedCreature->UseFeed(m_user, stomachValue))
        {
            CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
            // [b28d-solo P0/P183] ORIG 0x8339e1b `mov -0x1c(%ebp),%ebx` 后
            // 0x8339e27 外联 call GetUid，uid 无具名槽 ⇒ 内联实参。
            // ORIG 0x8339e3f `xor $1;test;je <成功>`：失败块 fall-through、
            // 判定整体内联（无具名 ok 局部，4.4 会槽化 cmpb 反劣）。
            if (!sender.UseFeedCreature(
                    reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(),
                    stomachValue))
            {
                // P227：ORIG 0x8339e54 vararg 在 ctor 前求值并跨 ctor 保活 %ebx。
                cMyTrace(__PRETTY_FUNCTION__, 0xab5, 5)(
                    "cannot write db uid(%d)\n",
                    m_user->get_unique_id() & 0xffff);
                return false;
            }
            sender.Send();
            return true;
        }
    }
    return false;
}
