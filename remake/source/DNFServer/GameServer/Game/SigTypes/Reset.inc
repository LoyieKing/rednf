void CHARAC_DATA::Reset()       // ORIG W 0x822ec9e
{
    // this+0x60 起 _Additioal_info（步长 0x52，见 CCharacter.cpp ctor 注释）
    m_add.clear();
    memset(m_data + 0x04, 0, 0x1e);      // ORIG +0x0b2 行
    memset(m_data + 0x34, 0, 0x19);      // ORIG +0x0d0 行
    memset(m_data + 0x138, 0, 0x53e);    // ORIG +0x0ee 行（至 +0x676，不含 m_schoolNo）
    memset(m_data + 0x0b9, 0, 0x10);     // ORIG +0x10d 行
    m_unkB2.m_b124 = 0;                  // ORIG +0x12e..+0x131：逐字节清零簇
    m_unkB2.m_b129 = 0;
    m_unkB2.m_b12a = 0;
    m_unkB2.m_b12b = 0;
    m_unkB2.m_b12c = 0;
    m_unkB2.m_b12d = 0;
    m_unkB2.m_b12f = 0;                  // 注意：+0x12e 未清（ORIG 跳过）
    m_unkB2.m_i130 = 0;                  // ORIG +0x139：dword 清零（movl $0,0x130(%eax)）
