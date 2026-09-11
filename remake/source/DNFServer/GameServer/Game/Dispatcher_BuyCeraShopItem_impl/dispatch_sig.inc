int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b28d-59 判词] 封 N82（ORIG 0x081fd396，§6.2 巨件三件套）：P167 调用点差集
    // 96/96 CLEAR（0 缺 0 多）、指令 1010/1002、帧 sub $0x7fb0 vs $0x7fbc；残差=槽序
    // /callee-saved 组（push 序）。语义完成，byte 层封顶。
    // [P6S5 2026-09-11 本波落地 · P139 prett 轴] ORIG 0x081fd396 引用
    // `_ZZN26Dispatcher_BuyCeraShopItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__`
    // @0x8bcfb40 size 0x49（"virtual int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser*, PacketBuf&)"）。
    // 旧译函数头 `static const char kPretty[] = "…"`（逐字节相同手写串）⇒ 我方发
    // `_ZN…E7kPretty` 符号、缺 prett 符号。本波把 23 处 `kPretty` 全换
    // `__PRETTY_FUNCTION__` 并删该定义行：`nm` 实测符号名与 size(0x49) 与 ORIG 全等，
    // 件读数 1009/1002（gap −7）与 TU 四数 2/2/0/2/1 **均不变** ⇒ 保留（零 codegen）。
    // ORIG 帧（sub $0x7fbc）：msg = -0x4008（MSG_BUY_CERASHOP_ITEM 0x3f84，
    // **无 memset**，全部 get_* 原地写入；未初始化读忠实保留——ORIG 各
    // ProcessError 第 4 参直接读 msg+0x39（entry0 productNo 槽，可能未写入））；
    // mailReq = -0x7f90（MSG_CHECK_MAIL_RECEIVER 0x3f88）；recvLen = -0x84(int)；
    // 循环节变量 i=-0x40 j=-0x2c k=-0x28 x=-0x24 y=-0x20；
    // goods=-0x38 item=-0x34 rand=-0x30 seller=-0x3c secuErr=-0x44 mailRc=-0x1c。
    // PacketGuard pg = -0x80（state 门之后构造，其后全部代码在 try 区，
    // catch handler 0x081fe484 → dtor + _Unwind_Resume）。
    MSG_BUY_CERASHOP_ITEM msg;
    MSG_CHECK_MAIL_RECEIVER mailReq;
    int  recvLen;                    // -0x84：仅 buyFlag==1 分支写入/读取
    int  i;                          // -0x40
    int  j;                          // -0x2c
    int  k;                          // -0x28
    int  x;                          // -0x24
    int  y;                          // -0x20
    int  randIdx;                    // -0x30
    CCeraShopGoods* goods;           // -0x38
    CItem* item;                     // -0x34
    private_store::CPrivateStore* seller;  // -0x3c
    int  secuErr;                    // -0x44
    int  mailRc;                     // -0x1c

    // ---- acc_id trace（NumberToString + cMyTrace）----
    // [N1Free91to105 2026-09-11] P0/P227：ORIG 0x081f... `mov %eax,%ebx` 接
    // NumberToString 结果（跨 cMyTrace 构造保活、**无栈槽**），且构造 call 在
    // NumberToString 之后 ⇒ 原码是单语句临时链；具名 `char* idStr` 实测落
    // -0x18 槽并令构造 call 提前（帧 0x7fb0 vs ORIG 0x7fbc）。
    cMyTrace(__PRETTY_FUNCTION__, 0x8f66, 0)("Dispatcher_BuyCeraShopItem(m_id(%s))",
                                 NumberToString(user->get_acc_id(), 0));

    // ---- user/trade/state 校验 ----
    if (user == 0 || user->CheckInTrade()) return 0;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0x8f7b, __PRETTY_FUNCTION__, 0, 0);

    PacketGuard pg;                  // ORIG 0x081fd482（-0x80）

    // ---- private store 忙检查 ----
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
        return 0;
    }

    // ---- trade 复查 ----
    if (user->CheckInTrade())
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, (char)0);
        return 0;
    }

    // ---- get_short → msg+0x0d（buytype，ORIG get_short(short&) 直写消息槽）----
    if (!packet.get_short((short&)msg.m_scalars.m_buytype))
        return LineFunc(0x8fa9, __PRETTY_FUNCTION__, 0, 0);
    if ((short)msg.m_scalars.m_buytype < 0 || (short)msg.m_scalars.m_buytype > 1)
        return LineFunc(0x8fad, __PRETTY_FUNCTION__, 0, 0);

    // ---- get_byte → msg+0x0f（buyFlag）----
    if (!packet.get_byte(msg.m_scalars.m_buyFlag))
        return LineFunc(0x8fb7, __PRETTY_FUNCTION__, 0, 0);

    // ---- buyFlag==1 → get_int(recvLen)+get_str(msg+0x10, 0x1e, recvLen) ----
    // ORIG get_str 目标 = msg+0x10（收件人名区，即 m_pad10 的 0x1e 字节），非独立缓冲。
    if (msg.m_scalars.m_buyFlag == 1)
    {
        if (!packet.get_int(recvLen))
            return LineFunc(0x8fbb, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(&msg.m_scalars.m_pad10[0], 0x1e, recvLen))
            return LineFunc(0x8fbc, __PRETTY_FUNCTION__, 0, 0);
    }

    // ---- buyFlag==1 && buytype==1 → 拒绝 ----
    if (msg.m_scalars.m_buyFlag == 1 && (short)msg.m_scalars.m_buytype == 1)
        return LineFunc(0x8fc2, __PRETTY_FUNCTION__, 0, 0);

    // ---- 整合 PvP base channel / 渠道类型拦截 ----
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // ORIG 0x081fd749 cmp $0xf; sete; test; je：物化 (type == 0xf) 后取反。
        if (!(G_GameWorld()->GetChannelType() == 0xf))
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
        if (msg.m_scalars.m_buyFlag == 1)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
    }

    // ---- 计费 trace（NEXON BILLING；ORIG 0x081fd7fe 先缓存 buyFlag→-0x7fa4，
    //      右→左求值：name→edi、no→esi、acc_id→NumberToString→ebx）----
    unsigned int buyType = (unsigned int)msg.m_scalars.m_buyFlag;   // -0x7fa4
    cMyTrace trB(__PRETTY_FUNCTION__, 0x9004, 0);
    trB("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d)",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacNo(),
        user->getCurCharacName(), buyType);

    // ---- get_byte → msg+0x36（itemCount）：bad = !(读成功 && !=0 && <=0x23) ----
    // ORIG 0x081fd8a4 起物化判定；失败走 ProcessError(1,0,msg.entry0.productNo,1,1,0)
    // （entry0 此时未写入 → 忠实读未初始化 msg+0x39）。
    if (!(packet.get_byte(msg.m_scalars.m_itemCount) == 1 &&
          msg.m_scalars.m_itemCount != 0 &&
          msg.m_scalars.m_itemCount <= 0x23))
    {
        GlobalData::s_pCeraShop->ProcessError(
            user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
        return 0;
    }

    // ---- 逐条解析（条目基址 msg+0x30、步长 0x1cf；ORIG 每字段现算地址）----
    for (i = 0; i < (int)msg.m_scalars.m_itemCount; ++i)
    {
        if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_avatarType0))
            return LineFunc(0x9017, __PRETTY_FUNCTION__, 0, 0);
        // +0x08：ORIG 走 get_byte(char&) 重载（0x0858cf22）。
        if (!packet.get_byte((char&)msg.m_entrySection.m_arr[i].m_avatarType1))
            return LineFunc(0x9018, __PRETTY_FUNCTION__, 0, 0);
        // +0x09：ORIG 走 get_int(unsigned int&) 重载（0x0858d240）。
        if (!packet.get_int((unsigned int&)msg.m_entrySection.m_arr[i].m_productNo))
            return LineFunc(0x9019, __PRETTY_FUNCTION__, 0, 0);
        // +0x0d subCount：失败分支 ORIG **不是** LineFunc，而是 ProcessError(1,...)
        if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_subCount))
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }
        if (msg.m_entrySection.m_arr[i].m_subCount > 0x23)
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 无子项且 avatarType0==0xff 时做头像选项随机（ORIG 0x081fdb37）--
        if (msg.m_entrySection.m_arr[i].m_avatarType0 == 0xff &&
            msg.m_entrySection.m_arr[i].m_subCount == 0)
        {
            goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(
                msg.m_entrySection.m_arr[i].m_productNo);
            if (goods == 0)
                return LineFunc(0x9032, __PRETTY_FUNCTION__, 0, 0);
            item = G_CDataManager()->find_item(goods->GetItemIdx());
            if (item != 0 && item->IsAvatarItem())
            {
                randIdx = get_rand_int(((CEquipItem*)item)->getAvatarOptionSize());   // B7
                // ORIG 0x081fdc24: `cmpl $0x63,-0x30(%ebp); jg skip`（有符号）
                // ⇒ 上限为 0x63(99)，randIdx>99 才跳过；原写 <=100 多放行 100 这一越界值。
                if (randIdx <= 99)
                    msg.m_entrySection.m_arr[i].m_avatarType0 = (unsigned char)randIdx;
            }
        }

        // -- 子项表 (stride5，基址 entry+0x0e) --
        for (j = 0; j < (int)msg.m_entrySection.m_arr[i].m_subCount; ++j)
        {
            // ORIG get_int(unsigned long&)（0x0858d27e）。
            if (!packet.get_int((unsigned long&)msg.m_entrySection.m_arr[i].m_subEntries[j].m_itemIdx))
                return LineFunc(0x9045, __PRETTY_FUNCTION__, 0, 0);
            if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_subEntries[j].m_value))
                return LineFunc(0x9046, __PRETTY_FUNCTION__, 0, 0);
        }

        // -- 选项数 optCount（entry+0xbd；ORIG 0x081fdd6c 物化：失败或 >=0x24
        //    统一走 ProcessError(1,...)，失败路径并非 LineFunc）--
        if (!(packet.get_byte(msg.m_entrySection.m_arr[i].m_optCount) == 1 &&
              msg.m_entrySection.m_arr[i].m_optCount <= 0x23))
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 选项表 (stride8，基址 entry+0xbe) --
        for (k = 0; k < (int)msg.m_entrySection.m_arr[i].m_optCount; ++k)
        {
            if (!packet.get_item_idx(msg.m_entrySection.m_arr[i].m_optEntries[k].m_itemUid))
                return LineFunc(0x9053, __PRETTY_FUNCTION__, 0, 0);
            if (!packet.get_short(msg.m_entrySection.m_arr[i].m_optEntries[k].m_a))
                return LineFunc(0x9054, __PRETTY_FUNCTION__, 0, 0);
            if (!packet.get_short(msg.m_entrySection.m_arr[i].m_optEntries[k].m_b))
                return LineFunc(0x9055, __PRETTY_FUNCTION__, 0, 0);
        }

        // -- 重复选项校验（同 itemUid(4B) + 同 a(2B) 视为重复）--
        for (x = 0; x < (int)msg.m_entrySection.m_arr[i].m_optCount; ++x)
        {
            for (y = x + 1; y < (int)msg.m_entrySection.m_arr[i].m_optCount; ++y)
            {
                if (msg.m_entrySection.m_arr[i].m_optEntries[x].m_itemUid ==
                        msg.m_entrySection.m_arr[i].m_optEntries[y].m_itemUid &&
                    msg.m_entrySection.m_arr[i].m_optEntries[x].m_a ==
                        msg.m_entrySection.m_arr[i].m_optEntries[y].m_a)
                    return LineFunc(0x905e, __PRETTY_FUNCTION__, 0, 0);
            }
        }

        // -- 私人商店娃娃（isDollCreated）拦截：product_no==0x1caad --
        seller = private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(user);
        if (seller != 0 && seller->isDollCreated() &&
            msg.m_entrySection.m_arr[i].m_productNo == 0x1caad)
        {
            // ORIG 0x081fe0d9：第 4 参取 -0x3fcf（**entry0** productNo，非 entry_i）。
            GlobalData::s_pCeraShop->ProcessError(
                user, 0xe7, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 计费 trace（含 product_no；ORIG 0x081fe13d product→-0x7fa0、
        //    buyFlag→-0x7f9c 两独立临时槽）--
        unsigned int productNo = (unsigned int)msg.m_entrySection.m_arr[i].m_productNo;
        unsigned int buyType2  = (unsigned int)msg.m_scalars.m_buyFlag;
        cMyTrace trP(__PRETTY_FUNCTION__, 0x9070, 0);
        trP("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d), product_no(%d)",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacNo(),
            user->getCurCharacName(), buyType2, productNo);
    }

    // ---- ServiceRestrictManager::isRestricted(user, (Enum)1, 0x11) ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(user, (RestrictCategory::Enum)1, 0x11))
    {
        // ORIG 0x081fe227：第 4 参 entry0 productNo、末参 movsbl(buyFlag)。
        GlobalData::s_pCeraShop->ProcessError(
            user, 0xd1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo,
            1, 1, (char)msg.m_scalars.m_buyFlag);
        return 0;
    }

    // ---- CSecu_ProtectionField::Check(user, 0x38) ----
    secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x38);
    if (secuErr != 0)
    {
        // ORIG 0x081fe2c4：GetCashShopErr 结果 movzwl 直传错误码槽，无具名局部。
        GlobalData::s_pCeraShop->ProcessError(
            user,
            (unsigned short)GlobalData::s_pSecuProtectionField->GetCashShopErr(secuErr),   // B4
            0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
        return 0;
    }

    // ---- 结算：先倒计时币副本检查，再 buyFlag==1（邮件赠送）/普通购买 ----
    // ORIG 直接传已读满的唯一消息对象（先前版本 memset+重组 buyMsg 属语义缺口）。
    if (useCountDownCoinInFreeCoinDungeon(user, msg))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, (char)0);
        return 0;
    }

    if (msg.m_scalars.m_buyFlag == 1)
    {
        // MSG_CHECK_MAIL_RECEIVER::set<ENUM_MAILBOX_CHECKTYPE_1, MSG_BUY_CERASHOP_ITEM>
        // 权威模板（CGameManager.h，ORIG _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE1E21MSG_BUY_CERASHOP_ITEMEEvRKT0_）：
        // *(int*)this=TYPE; memcpy((char*)this+4,&msg,sizeof(MSG_BUY_CERASHOP_ITEM)=0x3f84)。
        // MSG_CHECK_MAIL_RECEIVER 为全局作用域类型（ORIG mangled _ZN23MSG_CHECK_MAIL_RECEIVER... 无 WongWork）。
        // WongWork::CMailBoxHelper::ReqDBCheckReceiver 确在 WongWork 命名空间。
        // ORIG 0x081fe388：set 目标 = 栈对象（-0x7f90）；第 2 参 = msg+0x10 收件人名区。
        mailReq.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_1>(msg);
        mailRc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(
            user, &msg.m_scalars.m_pad10[0], &mailReq);
        if (mailRc != 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x90c2,
                                  "[ITEM_GIFT] Fail charac_no[%u] error_code[%d]",
                                  user->get_charac_no(-1), mailRc);
            user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x43, (unsigned short)(mailRc & 0xffff));
        }
        return 0;
    }

    // ---- 普通购买：ItemVendingMachine::BuyItem（B2）----
    if (!GlobalData::s_pItemVendingMachine->BuyItem(user, msg))
        return LineFunc(0x90ce, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}