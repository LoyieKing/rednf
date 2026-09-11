// ORIG 0x8377696：标量逐字段复制 + m_pair18/m_pair38 整块拷贝 + 三个 vector 拷贝构造。
// [b28d-solo-STAssignMonster] C1ERKS_：union 具名成员整体拷贝复现 ORIG 4.1
// 「dest 缓存 %ecx + 高低半读交错」8 字节块拷贝（P246，0x83776e5/0x8377833）。
STAssignMonster::STAssignMonster(const STAssignMonster& other)
    : m_0(other.m_0), m_4(other.m_4), m_8(other.m_8), m_c(other.m_c),
      m_10(other.m_10), m_14(other.m_14), m_pair18(other.m_pair18),
      m_20(other.m_20), m_24(other.m_24), m_vec28(other.m_vec28),
      m_34(other.m_34), m_pair38(other.m_pair38),
      m_vec40(other.m_vec40), m_4c(other.m_4c), m_50(other.m_50),
      m_54(other.m_54), m_55(other.m_55), m_58(other.m_58),
      m_vec5c(other.m_vec5c)
{
}