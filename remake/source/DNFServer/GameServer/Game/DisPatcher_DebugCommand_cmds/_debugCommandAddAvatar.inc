bool DisPatcher_DebugCommand::_debugCommandAddAvatar(CUser* user,
                                                     MSG_DEBUG_COMMAND& msg)
{
    if (user->isGMUser() != 1)
        return false;

    char dateKey[0x18];                                         // -0x4c
    memset(dateKey, 0, sizeof(dateKey));                        // +0x28 memset 0x18
    time_t now = time(0);                                       // +0x43 time(NULL)
    struct tm broken;                                           // -0x80
    struct tm* p = localtime_r(&now, &broken);                  // +0x55 localtime_r
    // env+0x1b0 = Server_Envir(+0x198).m_channelNo(+0x18)
    OS_API::snprintf(dateKey, 0x14, "D%02d%02d%02d%02d%010d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,  // +0x1b0（具名字段）
                     (p->tm_year + 0x76c) % 2000,               // +0xa3 (yr+1900)%2000
                     p->tm_mon + 1,
                     p->tm_mday,
                     (unsigned int)user->getCurCharacNo());     // %010d

    CItem* item = G_CDataManager()->find_item((int)msg.m_arg);  // +0x106 find_item(msg[0xe])
    // ORIG `mov $1; jmp; mov $0; test; je <ret0>` ⇒ 正条件包裹（P214①）。
    if (item != 0 && item->IsAvatarItem())                      // +0x114 vtbl+0x10 虚槽
    {
        int slot = 0;                                           // ORIG +0x13a movl $0,-0x1c
        // P227：ORIG 先求 usablePeriod(%ebx)/periodKind(%esi) 再 ctor。
        cMyTrace(g_dbgCmdPrettyAddAvatar, 0x231, 0)(
            "[DEBUG_COMMAND_ADD_AVATAR] Param.Sub.AddAvatar.period : %d, "
            "item->getUsablePeriod() : %d",
            (unsigned int)(unsigned char)msg.payload.avatar.periodKind,
            item->getUsablePeriod());                           // +0x145 thunk

        if (msg.payload.avatar.periodKind == 0)                 // +0x1a3 分支一
        {
            slot = user->getCurCharacInvenW()->AddAvatarItem(   // +0x221
                (int)msg.m_arg,
                item->getUsablePeriod(),                        // 分支一重新调用
                /*b*/ false,
                /*c*/ (char)msg.payload.avatar.slotByte,        // sbyte 槽位口径
                /*d*/ (char)-1,
                /*agency*/ dateKey,
                static_cast<eAvatarItemAddReason>(9),
                /*e*/ false,
                /*f*/ msg.payload.avatar.count);                // u16 数量
        }
        else                                                    // +0x22e 分支二
        {
            if (msg.payload.avatar.periodKind == 0x63)          // 哨兵 99 → 0
                msg.payload.avatar.periodKind = 0;
            slot = user->getCurCharacInvenW()->AddAvatarItem(   // +0x2b5
                (int)msg.m_arg,
                (int)msg.payload.avatar.periodKind,
                /*b*/ false,
                /*c*/ (char)msg.payload.avatar.slotByte,
                /*d*/ (char)-1,
                /*agency*/ dateKey,
                static_cast<eAvatarItemAddReason>(9),
                /*e*/ false,
                /*f*/ msg.payload.avatar.count);                // u16 数量
        }

        // ORIG +0x2c1 `je <尾 mov$0 块>`：成功条件正向包裹，失败块后置
        // （P222/P59；早返形 4.4 恒把 mov$0 放就地、成功块后置）。
        if (slot != -1)
        {
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                     static_cast<ENUM_ITEMSPACE>(1), slot);
            return true;
        }
        return false;
    }
    return false;
