// ===========================================================================
// WindowsExeFile —— 0x085736be..
// ===========================================================================
// [b28c-lane17] ORIG 0x085736be：先构造 m_sections(+8)/m_commonSections(+0x14)，
// 再体赋值 m_size=0/m_image=0（标量不进初始化列表）。
WindowsExeFile::WindowsExeFile()
    : m_sections(), m_commonSections()
{
    m_size = 0;
    m_image = 0;
}