bool CMailBoxHelper::_DoGetPackageAvatar(CUser* user, CMailBox::CMail const* mail,
                                         std::vector<std::pair<int, int> >& items,
                                         bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855419c：GetPackageItem 按值拷入本地（sret -0x4d），
    // 后续逐字段 push 传参（ORIG 无指针缓存槽）。
    Inven_Item pkg = mail->GetPackageItem();
    if (!mail->isAvatarPackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }
    int frame_pad10 = 0; // P31：ORIG movl $0,-0x10 dead local
    (void)frame_pad10;
    if (bReset)
    {
        register const int uid = pkg.m_addInfo2; // +0x07 avatar uid（跨调用 %ebx）
        if (user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(uid) == -1)
            return false;
        // ORIG +0x116：`shr $0x1f,%eax; test %al` 判负（探针 47 实证
        // (f() >> 31) != 0 形复现；`< 0` 恒 js/jns 单跳）。
        if ((user->getCurCharacInvenW()->tryInsertAvatarIntoInventory(pkg) >>
             31) != 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
    }
    else
    {
        int slot = 0;
        slot = user->getCurCharacInvenW()->insertAvatarIntoInventory(
            pkg, (eAvatarItemAddReason)5, true);
        // [2026-09-09 SweepCMailBoxHelper-2 翻案] 旧判词「槽值 shr 两步不可造」
        // 作废：ORIG 0x0855436e `mov -0xc(%ebp),%eax; shr $0x1f,%eax;
        // test %al,%al; je` 由 while 形复现（P225：body 必 return ⇒ 与 if
        // 语义等价；4.4 对 if 形恒折 `cmpl $0x0,槽; jns`）。探针 /tmp/p_mail/t.cpp
        // f2 逐条同形（f1/f3 为折叠反例）。
        while ((slot >> 31) != 0)
            return false;
        user->getCurCharacInvenW()->SendAvatarEvent(0x36, pkg.m_addInfo2, 0, slot + 10, 0);
        register const char* sysName =
            g_scriptStringManager_.findString(4, "game_server_msg_221", 0);
        if (std::strcmp(mail->GetSenderName(), sysName) == 0)
            DB_InsertCeraItemHistory::makeRequest(
                pkg.m_addInfo2, mail->GetSenderCharacNo(), user->getCurCharacNo(), 1);
        else
            DB_InsertCeraItemHistory::makeRequest(
                pkg.m_addInfo2, mail->GetSenderCharacNo(), user->getCurCharacNo(), 0);
    }
    return true;
