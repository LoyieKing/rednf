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
