int CMailBoxHelper::ReqDBSendNewMail(CUser* user, Inven_Item const& item,
                                     unsigned int nDestCharacNo, unsigned int nAmount,
                                     bool bA, const char* pTitle, int nTitleLen)
{
    // ORIG 0x08555ac8：转发 _DoSendNewMail(type=0)
    return _DoSendNewMail(user, item, nDestCharacNo, nAmount, bA, pTitle,
                          nTitleLen, 0);
