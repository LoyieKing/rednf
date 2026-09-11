// ORIG reset 0x0814aa16 逐字段清：movw +0/+2、movl +8、movb +0xc、movl +0x10/+0x14
// （不动 +0x04/+0x18 起字段）；旧 memset 0x48 语义与形态均不符。
void _Quest_Authen_Data::reset()
{
    m_characNo = 0;
    m_characClass = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
}