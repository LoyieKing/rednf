void CUser::DBUpdateDBLogItem() const
{
    // b28c-L5：ORIG 0x0868950x 增量段调
    //   `_ZN9__gnu_cxx17__normal_iteratorIPKN5CUser14_DBLogItemInfoESt6vectorIS2_SaIS2_EEEppEi`
    //   （**后置** ++，带独立 sret 临时槽 -0x1c：`lea -0x1c; movl $0,0x8(%esp);
    //   lea -0x24,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call ppEi; sub $4,%esp`）；
    //   4.4 对 `++it` 走 ppEv（返回引用）⇒ 少 sret 槽 + 少 `sub $0x4,%esp`（ndiff 10）。
    //   改后置形后该 sret 槽本身即把迭代器阶梯顶到 ORIG 位置 ⇒ 原「技巧 31 dead
    //   slotPad」变为多余（会让 it 深一格到 -0x28、全帧平移），删除。
    for (std::vector<_DBLogItemInfo>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        DB_LogBuyItem::makeRequest(get_acc_id(), getCurCharacNo(), it->m_itemIdx,
                                   it->m_type, it->m_value);
    }
