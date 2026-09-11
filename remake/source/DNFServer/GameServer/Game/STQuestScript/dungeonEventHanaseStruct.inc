// ORIG T 0x8a60f78（0x95）：0x14 字节标量区清零（+0x10 为字节）+ m_str20 构造
// + m_field24 清零 + 11 个 dialog 指针清零（i=0..10，P151）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0), m_field10(0),
      m_field14(0), m_field18(0), m_field1c(0), m_str20(), m_field24(0)
{
    for (int i = 0; i <= 10; ++i)
        m_dialogs[i] = 0;
}