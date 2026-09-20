bool CUserCharacInfo::needCurCharacCheckOverEquip()
{
    if (!m_selected)
        return 0;
    // [Q5-InterDisp 2026-09-11 · Main 授权] 字段已同型 bool ⇒ 直返（ORIG 0x08696ca4
    //   `movzbl 0x127b(%eax),%eax; jmp`，无 uchar→bool 归一化 test/setne）。
    return m_selected->m_needCheckOverEquipItem;
}