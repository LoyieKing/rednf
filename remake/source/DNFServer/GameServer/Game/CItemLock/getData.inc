void CItemLock::getData(char* pData) const
{
    // [b28d-solo-CItemLock] AE：旧 DIFF（N7 偏移 add 序 + N84/N90 拷贝读序）
    // 作废 —— ①具型 POD 指针 `data + idx` 复现 ORIG 0x85419da `mov i,%eax;
    // shl $3,%eax; mov %eax,%ebx; add -0x10(%ebp),%ebx`（同 loadData）；②POD 视图
    // 赋值复现 `mov 0x8(%eax),%edx; mov 0x4(%eax),%eax; mov %eax,(%ebx);
    // mov %edx,0x4(%ebx)` 先读全再写全（探针 fD）。语义为同一 8 字节块拷贝。
    stItemLockInfoPod* data = (stItemLockInfoPod*)pData;   // ORIG 存槽 -0x10
    for (std::map<unsigned char, stItemLockInfo>::const_iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x6c, "itemlock error index %d", idx);
            continue;
        }
        // 直接成员访问（保留 const_iterator::operator-> 调用链，P103）⇒ 4.4 把
        // +4/+8 折进 disp 并发 movdi 展开（ORIG 0x85419da 段逐条同形）。
        *(data + idx) =
            ((const stItemLockInfoPairPod*)it.operator->())->m_second;
    }
