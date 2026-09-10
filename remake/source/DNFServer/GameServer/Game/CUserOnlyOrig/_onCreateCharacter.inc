void CUser::_onCreateCharacter(unsigned int characNo, char* name, char job)
{
    (void)name;
    if (job == 4) {
        // [b28d-22 修法 P197④/P183] ORIG 0x0864a45d `cmp $0x4936f350,%eax; setle %al;
        // test; je`（嵌套 if + 有符号 <= 复现 setle，探针 /tmp/p22_g1b.cpp occ2 逐条同构；
        // 具名 unsigned 局部 + `< 0x4936f351u` 反产 ja 直跳）；getItemList 内联体经
        // m_itemList 成员直访（ORIG 0x0864a47f 仅 `call G_CDataManager; mov 0xc(%eax),%eax`）。
        if ((int)GlobalData::s_systemTime_.getCurSec() <= 0x4936f350) {
            Inven_Item item;
            item.m_addInfo = 0x1d19;
            G_CDataManager()->m_itemList->create_item(
                (ENUM_ITEM_CREATE_TYPE)3, item, 1);
            if (item.m_addInfo == -1) {
                return;
            }
            char title[0x15];
            char body[0x100];
            // ORIG 0x0864a4d6/0x0864a511/0x0864a559：findString 返回值免 s56/s57 槽
            // 直入 strncpy 实参、strlen 免 bodyLen 槽直入邮件实参（P0）。
            strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_56", 0), 0x14);
            strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_57", 0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0, characNo, body, (int)strlen(body), 0,
                (ENUM_SERVER_GROUP)0, false, false);
        }
    }
    _processUnlimitChallenge(characNo, job);
