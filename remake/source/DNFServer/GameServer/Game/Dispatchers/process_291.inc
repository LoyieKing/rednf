int Dispatcher_ImageCommunicationEquipmentUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    const int err = check_error(user, msg);
    out.m_4 = err;
    if (err == 0)
    {
        const int price = *(int*)G_CDataManager()->GetImageCommnicationData();
        user->getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x30, true);
        out.m_8 = *(int*)G_CDataManager()->GetImageCommnicationData();
        out.m_c = user->getCurCharacMoney();
    }
    return 0;
