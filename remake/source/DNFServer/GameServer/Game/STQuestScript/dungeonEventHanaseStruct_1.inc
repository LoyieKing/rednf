// ORIG T 0x8a6100e：逐字段拷贝 + 深拷贝 11 个 dialog 串
// （new std::string + operator=，非拷贝构造；ORIG asm _Znwj→SsC1→SsaSERKSs 实证）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct(
    const dungeonEventHanaseStruct& o)
    : m_field0(o.m_field0), m_field4(o.m_field4), m_field8(o.m_field8),
      m_fieldc(o.m_fieldc), m_field10(o.m_field10), m_field14(o.m_field14),
      m_field18(o.m_field18), m_field1c(o.m_field1c), m_str20(o.m_str20),
      m_field24(o.m_field24)
{
    for (int i = 0; i <= 10; ++i)
    {
        m_dialogs[i] = 0;
        if (o.m_dialogs[i] != 0)
        {
            m_dialogs[i] = new std::string;
            *m_dialogs[i] = *o.m_dialogs[i];
        }
    }
}