void stEnamyDropAvatarInfo_t::clear()
{
    m_field00 = 0;                 // movl $0,(%eax)
    m_field04 = 0;                 // movl $0,0x4
    for (int i = 0; i <= 3; ++i)
        m_field08v[0][i] = 0;      // [base+8+4i]
    for (int i = 0; i <= 4; ++i)   // [base+8+4(i+4)]：ORIG `lea 0x4(%edx)` 形
        m_field08v[1][i] = 0;
    m_field2c = 0;
    m_elements.clear();            // vector<element>::clear(+0x30)
