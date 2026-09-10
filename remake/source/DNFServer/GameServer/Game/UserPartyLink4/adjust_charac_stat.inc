void CUser::adjust_charac_stat()
{
    _Additioal_info calc;
    calc.clear();
    calc_lev_stat(calc);

    const _Additioal_info* before =
        static_cast<const _Additioal_info*>(getCurCharacAddInfoRefR());
    cMyTrace(__PRETTY_FUNCTION__, 0x44d1, 0)(
        "before adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        before->m_field00, before->m_field04,
        (int)before->m_field08, (int)before->m_field0a,
        (int)before->m_field0c, (int)before->m_field0e,
        before->m_field42,
        (short)before->m_field10[0], (short)before->m_field10[1],
        (short)before->m_field10[2], (short)before->m_field10[3]);

    _Additioal_info* current =
        static_cast<_Additioal_info*>(getCurCharacAddInfoRefW());
    // P83/N84：ORIG 0x08664911..0x08664987 为 0x52 字节结构整体赋值的内联展开
    // （对齐 peel 1B/2B + rep movsl + 尾 2B/1B，共 40 条、无 memcpy call）⇒
    // 源码必须是结构整体赋值让 4.4 展开；写 memcpy 恒退化为 `call memcpy`（gap -35 根因）。
    *current = calc;

    cMyTrace(__PRETTY_FUNCTION__, 0x44d7, 0)(
        "after adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        current->m_field00, current->m_field04,
        (int)current->m_field08, (int)current->m_field0a,
        (int)current->m_field0c, (int)current->m_field0e,
        current->m_field42,
        (short)current->m_field10[0], (short)current->m_field10[1],
        (short)current->m_field10[2], (short)current->m_field10[3]);
