// ORIG T 0x081a0fe4 sz 0x97（SendMail.md 全文已读）：
// m_field10 < 0 → send_mail(user,p2,p3,p4,m_field0c)；
// 否则 p4==7 → p3=5；m_field10 == p4 → send_mail(user,p2,p3,p4,m_field0c)。
// 依赖 GlobalInstance<Arad_GiftItem_Set>::inst()（ORIG W 0x081a1155）。
void CAradRyosikaEvent::SendMail(CUser* user, int param2, int param3, int param4)
{
    if (m_field10 < 0)
    {
        GlobalInstance<Arad_GiftItem_Set>::inst()->send_mail(
            user, param2, param3, param4, m_field0c);
        return;
    }
    if (param4 == 7)
        param3 = 5;
    if (m_field10 == param4)
    {
        GlobalInstance<Arad_GiftItem_Set>::inst()->send_mail(
            user, param2, param3, param4, m_field0c);
    }
}