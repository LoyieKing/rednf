// ORIG W 0x08697792 sz 0xc：movzbl (this) 直返全字（无 setne）——m_flag/返回
// 均 bool 时同形（P32 直返）；调用点 xor 形按 P292 要求 bool 返回。
bool CLevelRewardTableMgr::IsOn()
{
    return m_flag;
}