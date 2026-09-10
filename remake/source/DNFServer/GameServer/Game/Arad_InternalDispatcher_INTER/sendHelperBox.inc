bool Arad_INTER_kAradJumpingCharacter::sendHelperBox(CUser* user,
                                                     unsigned char ticketType,
                                                     unsigned char growValue)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] NEAR→封 N7/P78（ORIG 0x0819973e）：146=146 条、帧 0x1b8 全等、
    // 槽基 item@-0x6e/body@-0x16e/title@-0x31/rewardIndex@-0x1c 全等；唯一残差 =
    // callee-saved 角色互换（ORIG job→%esi、ticketType→%edi；4.4 恒 job→%edi、
    // ticketType→%esi，压参 0x20/0x1c 随之）。实测三旋钮均不翻：①内联实参
    // ②裸 register 两声明序 ③去 rewardIndex 冗余 cast ⇒ P248 条件②/N109 封顶。
    // 真语义 bug 已修：getCurRewardItem 第三参原传独立局部 itemId，ORIG 传
    // &item.m_addInfo（Inven_Item+0x02 物品 idx 出参）⇒ 原码 create_item 造 0 号物品。
    Inven_Item item;                                        // -0x6e
    char body[256];                                         // -0x16e
    char title[21];                                         // -0x31
    int rewardIndex;                                        // -0x1c

    if (ticketType == 1)
    {
        rewardIndex = (int)growValue;
        strncpy(title, g_scriptStringManager_.findString(
                           4, "jumping_character_title_01", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(
                          4, "jumping_character_mail_01", 0), 0xff);
    }
    else
    {
        rewardIndex = (int)growValue + 0x10;
        strncpy(title, g_scriptStringManager_.findString(
                           4, "jumping_character_title_02", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(
                          4, "jumping_character_mail_02", 0), 0xff);
    }

    // [b28d-solo] 真 bug 修复：ORIG 0x0819989a 第三参 = `lea -0x6e(%ebp); add $0x2`
    // 即 &item.m_addInfo（Inven_Item+0x02 物品 idx 出参）。原用独立局部 unsigned long
    // itemId ⇒ getCurRewardItem 结果写栈槽、item.m_addInfo 恒 0，create_item 造 0 号物品。
    // [b28d-solo] ORIG 0x081998b7 `xor $1; test %al; je <success>`：bool callee 裸形
    // `if (!call())` 直产（P168a①，勿经 bool 局部——4.4 会落槽 mov%al/cmpb）。
    // 负极性直落 error、success 为跳转目标（P222 正向包裹/P194 块序）。
    if (!G_CDataManager()->m_aradJumping.getCurRewardItem(
            user->get_charac_job(), rewardIndex,
            *reinterpret_cast<unsigned long*>(&item.m_addInfo)))
    {
        // [b28d-solo] ORIG 0x081998c2..0x08199909 右起求值：get_charac_job→%esi、
        // ticketType→%edi、get_charac_no→%ebx、get_acc_id→%eax 内联实参保活。
        register unsigned int jobArg = (unsigned int)user->get_charac_job();
        register unsigned int ticketArg = (unsigned int)ticketType;
        LogManager::logFormat
            (1, "localjapan/Arad_InterDispatcher.cpp",
             __PRETTY_FUNCTION__,
             0x5d5,
             "JUMPING CHARACTER : REWARD ERROR - m_id:%u, charac_no:%u, tickettype:%d, job:%d, growValue:%d",
             (unsigned int)user->get_acc_id(),
             (unsigned int)user->get_charac_no(-1),
             ticketArg,
             jobArg,
             rewardIndex);
    }
    else
    {
        // ORIG 直读 mgr+0xc（getItemList 虽头内 inline，4.4 -O0 仍折成 call）。
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
        item.m_addInfo2 = 1;                                // ORIG movl $1,-0x67(%ebp) 直写
        // [b28d-solo] ORIG 0x08199977..0x081999dc 右起求值：GetServerGroup→%ebx、
        // strlen→%esi、getCurCharacNo→%eax 全部内联实参（具名局部会落 3 槽 + 帧 +0x14）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 100000, (unsigned long)user->getCurCharacNo(), body,
            (int)strlen(body), 0, (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
        return true;
    }
    return false;
