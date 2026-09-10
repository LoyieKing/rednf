void CBoosterGage::make_postar(CUser* pUser, const Inven_Item& item,
                               const char* pAgency, int nPeriod)
{
    // [b28d-solo-CBoosterGage] 死局部把 szPost 顶到非 4 对齐槽 -0x13d（与 ORIG 同址），
    // 触发 4.4 `= {0}` 的对齐剥离 rep stos 慢路（P232 死局部 + N78 突破口）。
    char cPad1[1];
    char szPost[0x100] = {0};  // ORIG 内联 rep stos 0x100 字节（P214③/P253）
    char szTitle[0x14];
    strncpy(szTitle, g_ServerString_.GetServerString(0x436, NULL).c_str(), 0x14);
    strncpy(szPost, g_ServerString_.GetServerString(0x437, NULL).c_str(), 0xff);

    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == NULL)
        return;
    if (pItem->IsAvatarItem())
    {
        int nExpireDate = 0;
        if (nPeriod != 0 && nPeriod != 0x16d)
            nExpireDate = GlobalData::s_systemTime_.getCurSec() + nPeriod * 0x15180;
        WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
            pUser, pUser->getCurCharacNo(), item.m_addInfo, nExpireDate,
            0, -1, pAgency, szPost, (int)strlen(szPost));
    }
    else if (pItem->IsCreatureItem())
    {
        WongWork::CMailBoxHelper::ReqDBSendNewCreatureMail(
            pUser, pUser->getCurCharacNo(), item, szPost, (int)strlen(szPost));
    }
    else
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            szTitle, item, 0, pUser->getCurCharacNo(), szPost,
            (int)strlen(szPost), 0, (ENUM_SERVER_GROUP)pUser->GetServerGroup(), 0, 0);
    }
