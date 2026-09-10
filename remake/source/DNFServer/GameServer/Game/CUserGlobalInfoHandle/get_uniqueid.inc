int CUserGlobalInfoHandle::get_uniqueid()
{
    int idx = find_uniqueid();
    // [P225 while+break] ORIG 0x086ad924 `cmpl $0xffffffff,-0x4(%ebp); sete %al;
    // test %al,%al; je` 是 4.1 的 bool 物化后条件分支；4.4 对裸 `if` 恒折
    // `cmpl $-1,-0x4(%ebp); jne` 直跳。按 P225（CVillageAttacked/CDropItem 实证）
    // 改用 `while (cond) { body; break; }` 精确复现 setCC+test+je 四段物化。
    while (idx == -1)
    {
        m_next = 1;
        idx = find_uniqueid();
        break;
    }
    m_next = idx + 1;
    return idx;
