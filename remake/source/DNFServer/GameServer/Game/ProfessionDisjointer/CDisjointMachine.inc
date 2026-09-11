// ---- CDisjointMachine 六参 ctor（objdump 0x085d1ae8 忠实转写）：
//      IObject 基类 → vptr → Rectangle 默认构造 → name 默认构造 → 逐字段赋值 →
//      碰撞盒 {x-0x50, y-0x32, 0xa0, 0x64}。 ----
CDisjointMachine::CDisjointMachine(CUser* user, char const* name, int level,
                                   char grade, int x, int y)
{
    m_user              = user;                 // +0x14
    m_name              = name;                 // +0x18
    m_field1c           = level;                // +0x1c
    m_field20           = grade;                // +0x20（char）
    m_field24           = x;                    // +0x24（ORIG 全宽 mov，无 ushort 截断）
    m_field28           = y;                    // +0x28

    m_rect.m_pos.m_x    = m_field24 - 0x50;     // Rectangle +0x00（ORIG 成员回读）
    m_rect.m_width      = 0xa0;                 // +0x08
    m_rect.m_pos.m_y    = m_field28 - 0x32;     // +0x04（同上）
    m_rect.m_height     = 0x64;                 // +0x0c
}