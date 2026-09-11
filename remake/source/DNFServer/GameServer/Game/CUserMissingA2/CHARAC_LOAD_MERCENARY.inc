// ----------------------------------------------------------------------------
// 6b. ORIG C1 W 0x08694c08 sz 0x86（报告缺失，objdump 取证）：与 Reset 字段
// 集一致但顺序不同 —— 标量在前（+0/+4/+0x24/+0x28/+0x2c 清零、
// +0x30/0x31=0xff、+0x32=0xa），两个 memset 在后（+6,0x1e / +0x34,0x20）。
// ----------------------------------------------------------------------------
CHARAC_LOAD_MERCENARY::CHARAC_LOAD_MERCENARY()
{
    m_characNo = 0;
    m_level = 0;
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
    memset(m_name, 0, 0x1e);
    memset(m_avatarItems, 0, 0x20);
}