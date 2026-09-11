// ORIG _ZN8WongWork11Avatar_Item4initEv W 0x82fa35a：仅清 +0x00/+0x04 两个 int、
// memset(m_ipgAgencyNo,0,0x19)、memset(+0x3f,0,4)；emblem 区与 +0x43 之后不清。
void Avatar_Item::init()
{
    m_expireDate = 0;
    m_registTime = 0;
    memset(m_ipgAgencyNo, 0, 0x19);
    memset(&m_color1, 0, 4);
}