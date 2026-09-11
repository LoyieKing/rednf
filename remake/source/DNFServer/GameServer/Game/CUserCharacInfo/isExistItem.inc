bool CUserCharacInfo::isExistItem(int itemIdx)
{
    if (!m_selected)
        return false;
    // ORIG（0x8678084）`lea 0xdaa(%eax),%edx`/`lea 0xda(%eax)` 指针算术形态；
    // char 数组成员 decay 会退化为 add 改写 dest 寄存器（回退证据：转换实验），
    // 故保留指针算术但偏移改用具名字段 offsetof（防布局漂移，docs/
    // class_func_reports/CUserCharacInfo/isExistItem.md）。
    if (((const CCargo*)((const char*)m_selected +
                         offsetof(CharacInfoFields, m_cargo)))->check_item_exist(itemIdx) != -1)
        return true;
    if (((const CInventory*)((const char*)m_selected +
                             offsetof(CharacInfoFields, m_inven)))->check_item_exist(itemIdx) != -1)
        return true;
    return false;
}