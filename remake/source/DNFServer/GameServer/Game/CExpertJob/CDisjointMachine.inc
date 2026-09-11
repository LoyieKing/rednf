// ORIG 0x85d3d7e：IObject 基类构造 -> Rectangle(+0x04) -> CUser*+0x14 清零 ->
// std::string(+0x18) 默认构造。
CDisjointMachine::CDisjointMachine()
    : m_rect(), m_user(0), m_name(), m_field1c(0), m_field20(0),
      m_field24(0), m_field28(0)
{
}