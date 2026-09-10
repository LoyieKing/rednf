void Arad_IPGProcess::CashShopModeAvatar(CUser* user, unsigned long itemIdx,
                                         int a3, int a4, char c1, char c2,
                                         char const* str, int& out,
                                         unsigned int accId, int a10, char buyFlag)
{
    (void)a3;  // ORIG p3 在两分支均未使用（ebp 0x14 无读取）
    int len;   // ORIG -0x38（形参 spill 区之后）⇒ 头部声明（4.1 先声明深）

    if (buyFlag == 1)
    {
        // mangled _processAvatarGift(CUser*, i, i, c, c, PKc, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processAvatarGift(
            user, (int)itemIdx, a4, c1, c2, str, accId, a10);
        return;
    }

    // P52：receiver 链保持单语句，ORIG 把 c2/c1/itemIdx 先物化 edi/esi/ebx 跨 getter 保活。
    int slot = user->getCurCharacInvenW()->AddAvatarItem(
        (int)itemIdx, a4, false, c1, c2, str, (eAvatarItemAddReason)0, false, 0);
    if (slot < 0)
    {
        const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
        len = (int)strlen(msg);
        // P0：characNo 无槽往返 ⇒ 原码是实参内联（ORIG 无 -0xc 槽）。
        WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
            user, user->getCurCharacNo(), (int)itemIdx, a4, c1, c2, str, msg, len);
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
    }
