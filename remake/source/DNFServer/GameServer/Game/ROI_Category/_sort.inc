void ROI_Category::_sort()
{
    // tmpKey 与字段同型（short）：P211 同型 ⇒ 4.4 store LHS(this) 先，
    // 复现 ORIG `mov this; movzwl -0x4; mov %dx,0x2` 序（异型恒 RHS 先）。
    short tmpKey;
    unsigned char tmpMark;
    if (m_keys[0] > m_keys[1]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[0];                       // ORIG 共用 -0x4(%ebp)
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        tmpMark = m_marks[0];                     // ORIG 共用 -0x1(%ebp)
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
    if (m_keys[1] > m_keys[2]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[1];
        m_keys[1] = m_keys[2];
        m_keys[2] = tmpKey;
        tmpMark = m_marks[1];
        m_marks[1] = m_marks[2];
        m_marks[2] = tmpMark;
    }
    if (m_keys[0] > m_keys[1]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[0];
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        tmpMark = m_marks[0];
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
