bool CItemLock::CheckItemLock(unsigned char b) const
{
    // ORIG 0x8541a96：find 结果槽 -0x10 先于 end 临时槽 -0xc（P28 声明序即
    // 栈槽序）⇒ 两条语句（单表达式形把 find 槽排到 -0xc）。
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(b);
    // P56：ORIG `test;je;mov$1;jmp;mov$0` ⇒ 分支返回，勿直 return 比较表达式。
    if (it != m_lockMap.end())
        return true;
    return false;
}