// ORIG W 0x08a3d8c2：+0/-1、+4/0、+0x38/+0x3c/+0x40/-1，再 clear +8/+0x2c int vec 与 +0x14 string vec。
void STPassiveObjectDestroy::clear()
{
    m_type = -1;
    m_flag04 = 0;
    m_i38 = -1;
    m_i3c = -1;
    m_i40 = -1;
    m_ints08.clear();
    m_ints2c.clear();
    m_strs14.clear();
}