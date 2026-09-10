bool ARAD_ITEM::arad_process_item(CUser* user, Inven_Item const& item)
{
    if (user == 0 || item.m_addInfo == 0)
        return false;

    int restrictCode = -1;
    int code = 0;

    // ORIG 构造序：items vector → sysTime → GetRestrictCode。
    std::vector<Inven_Item> items;
    const int secTime = GlobalData::s_systemTime_.getCurSec();  // getCurSec()
    // P167：ORIG 直接读 G_CDataManager()+0xc（m_itemList）调 GetRestrictCode，
    // 不经 getItemList()（后者 -O0 未内联反产外联 call）
    const int restrictSrc = G_CDataManager()->m_itemList->GetRestrictCode(
        item.m_addInfo);

    WongWork::CCeraShop::stCeraShopItemParam_t param;

    // ORIG 第 3 参为 GetRestrictCode 的返回值、第 4 参为 code 出参引用；
    // curSec 以 long 低半传递（ORIG 仅赋值低半栈槽）。
    if (!GlobalData::s_pCeraShop->ProcessSpecialItem(
            user, item.m_addInfo, restrictSrc, code, secTime, true,
            items, param, '\0', '\0', false, 1))
    {
        // P47/P222：ORIG `cmp $-1; je <mail 外置块>`，update 直落 —— 故取反极性。
        if (restrictCode != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                     restrictCode);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, item, 0, user->getCurCharacNo(), true, msg, strlen(msg));
        }
    }
    else
    {
        LogManager::logFormat(
            1, kAradFuncFile, __PRETTY_FUNCTION__, 0x187,
            "Error process Item (charac:%u, item:%u, count:%d)",
            (unsigned int)user->getCurCharacNo(),
            (unsigned int)item.m_addInfo, item.m_addInfo2);
        return false;
    }
    return true;
