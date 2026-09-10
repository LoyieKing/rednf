bool CItemLock::loadData(CUser* user, char* pData)
{
    // [b28d-solo-CItemLock] AE：ORIG 0x08541714 `mov i,%eax; shl $3,%eax;
    // mov %eax,%edx; add -0x10(%ebp),%edx` —— 索引先物化入 %edx、data 作 add
    // 内存源。旧判 N7/N18a（13 形 char* 算术全产 LHS-first）作废：真因是原码
    // **具型指针局部** `stItemLockInfo* data`，`data + i` 由 sizeof=8 自然缩放出
    // 同一条 shl $3 链（探针 /tmp/b28d_solo_CItemLock/probe_add.cpp fF 逐条全等，
    // 同族 P255 数组名退化直传）。偏移/语义不变（stItemLockInfo 恰 8 字节）。
    stItemLockInfo* data = (stItemLockInfo*)pData;   // ORIG 0x854171d 存槽 -0x10
    for (int i = 99; i >= 0; --i)
    {
        PushItemLock(user, (unsigned char)(i + 1), data + i);
    }
    return true;
