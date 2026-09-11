// ORIG W 0x82f060a（size 0xa）：mov 0x0(%eax),%eax; ret —— 外联定义（P119）
unsigned int PartyInfo::Get()
{
    return (unsigned int)m_field00;
}