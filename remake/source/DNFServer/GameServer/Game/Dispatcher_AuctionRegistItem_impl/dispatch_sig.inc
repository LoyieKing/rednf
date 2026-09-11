// [b28d-51] 判词 N（ORIG 0x08213e8a，N82/N89 族）：dispatch 模板栈布局 +
// 临时槽阶梯跨版本差；语义/调用集齐，byte 层封顶。
int Dispatcher_AuctionRegistItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char payType;                                   // -0x45
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc294, __PRETTY_FUNCTION__, 0, 0);

    if (payType > 1)
        return LineFunc(0xc297, __PRETTY_FUNCTION__, 0, 0);

    PAY_TYPE pay = (PAY_TYPE)payType;                        // -0x30（0=gold，1=cera）

    // ---- cera 拍卖需角色等级达到 0x1f（31） ----
    if (pay == 1)
    {
        if (user->get_charac_level() < 0x1f)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x75, pay);
            return 0;
        }
    }

    // ---- 拍卖前置校验（代理是否运行 / state / Secu 0x35）；返回 <0 才继续 ----
    int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xba, pay);   // -0x2c
    if (preCheck >= 0)
        return preCheck;

    // ---- 服务限制 / 安全保护（0x32） ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 5))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd1, pay);
        return 0;
    }

    unsigned int secuErr =                                   // -0x28
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x32);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, (unsigned char)secuErr, pay);
        return 0;
    }

    if (pay == 1)
    {
        // ============ cera（点券）分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP ============
        PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP pck;            // -0x8d（ctor 0x0822e9b4）
        pck.char_idx = user->GetUID();
        pck.character_id = user->getCurCharacNo();

        if (packet.get_byte(pck.inven_type) != 1)
            return LineFunc(0xc2fa, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(pck.slot_no) != 1)
            return LineFunc(0xc2fc, __PRETTY_FUNCTION__, 0, 0);

        unsigned int itemId;                                 // -0x4c
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc302, __PRETTY_FUNCTION__, 0, 0);
        pck.item_id = itemId;

        if (packet.get_int(pck.add_info) != 1)
            return LineFunc(0xc305, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.price) != 1)
            return LineFunc(0xc306, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.instant_price) != 1)
            return LineFunc(0xc307, __PRETTY_FUNCTION__, 0, 0);

        // ---- cera 限价 / 物品段校验 ----
        if (IsPriceError(pck.price, pck.instant_price) || pck.instant_price > 0x895440)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }
        if (itemId < 0x28d288 || itemId > 0x28d299)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (pck.add_info > 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (pck.price != -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return LineFunc(0xc322, __PRETTY_FUNCTION__, 0, 0);
        }
        if (pck.instant_price < 0x32 || pck.instant_price > 0x895440)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
            return 0;
        }

        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }
    else
    {
        // ============ gold（金币）分支：PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA ============
        PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA pck;            // -0xca（ctor 0x0822e54e）
        pck.char_idx = user->GetUID();
        pck.character_id = user->getCurCharacNo();

        if (packet.get_byte(pck.inven_type) != 1)
            return LineFunc(0xc38a, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_short(pck.slot_no) != 1)
            return LineFunc(0xc38c, __PRETTY_FUNCTION__, 0, 0);

        unsigned int itemId;                                 // -0x50
        if (packet.get_int(itemId) != 1)
            return LineFunc(0xc392, __PRETTY_FUNCTION__, 0, 0);
        pck.item_id = itemId;

        if (packet.get_int(pck.add_info) != 1)
            return LineFunc(0xc395, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.price) != 1)
            return LineFunc(0xc396, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.instant_price) != 1)
            return LineFunc(0xc397, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(pck.unit_price) != 1)
            return LineFunc(0xc398, __PRETTY_FUNCTION__, 0, 0);

        // ---- ROI 类别：3 × (short high, char low)，直接读入包 +0x31 ----
        // ORIG 调 PacketBuf::get_short(short&) / get_byte(char&)：ROI_Category.h 的
        // m_keys/m_marks 为 unsigned，故以引用转型选中与 ORIG 相同的重载（地址计算不变）。
        for (int i = 0; i < 3; ++i)                          // -0x14
        {
            if (packet.get_short(*(short*)&pck.m_roiCategory.m_keys[i]) != 1)
                return LineFunc(0xc3a2, __PRETTY_FUNCTION__, 0, 0);
            if (packet.get_byte(*(char*)&pck.m_roiCategory.m_marks[i]) != 1)
                return LineFunc(0xc3a5, __PRETTY_FUNCTION__, 0, 0);
        }

        CItem* item = G_CDataManager()->find_item(itemId);   // -0x24
        if (item == 0)
            return LineFunc(0xc3aa, __PRETTY_FUNCTION__, 0, 0);

        // 随机选项装备（虚 vptr+0x4c IsRandomOption）：空 ROI high 槽置 0x7530（30000）
        if (item->IsRandomOption())
        {
            for (int i = 0; i < 3; ++i)                      // -0x10
            {
                if (pck.m_roiCategory.m_keys[i] == 0)
                    pck.m_roiCategory.m_keys[i] = 0x7530;
            }
        }
        pck.m_roiCategory._sort();

        // ---- 拍卖限购检查（CDataManager+0x4e8 = m_serverParameter.m_auction480） ----
        std::vector<stAuctionLimitItemParam_t>& limitVec =                     // -0x20
            G_CDataManager()->m_serverParameter.m_auction480;
        unsigned int curSec = GlobalData::s_systemTime_.getCurSec();           // -0x1c
        for (std::vector<stAuctionLimitItemParam_t>::iterator it = limitVec.begin();
             it != limitVec.end(); ++it)                                       // -0x54 / -0x44
        {
            if (it->m_itemId == -1 || it->m_itemId == itemId)
            {
                if (it->m_startSec < curSec && curSec < it->m_endSec)
                {
                    // 限购窗口内：结余包 header(1,0xba)|byte0|byte7|3×int
                    PacketGuard pg;                                            // -0x60
                    pg.put_header(1, 0xba);
                    pg.put_byte(0);
                    pg.put_byte(7);
                    pg.put_int(it->m_itemId);
                    pg.put_int(it->m_startSec);
                    pg.put_int(it->m_endSec);
                    pg.finalize(true);
                    user->Send(pg);
                    return 0;
                }
            }
        }

        // ---- 物品锁定 / 价格范围 / 可堆叠乘积校验 ----
        if (user->CheckItemLock(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)pck.inven_type), pck.slot_no))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0xd5, (PAY_TYPE)0);
            return 0;
        }
        if (pck.price < -1 || pck.instant_price < -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }
        if (pck.instant_price != -1 && pck.price >= pck.instant_price)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, (PAY_TYPE)0);
            return 0;
        }

        CItem* item2 = G_CDataManager()->find_item(itemId);  // -0x18
        if (item2 == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, pay);
            return 0;
        }
        if (item2->is_stackable())
        {
            // ORIG：call → xor $0x1,%eax → test %al,%al（bool 与 1 比较）
            if (IsValidMultiplyResult(pck.price, pck.add_info) != 1 ||
                IsValidMultiplyResult(pck.instant_price, pck.add_info) != 1)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x98, pay);
                return 0;
            }
        }

        GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }

    // ---- 收尾：GM 角色号延迟追踪 ----
    for (int i = 0; i < 5; ++i)                              // -0xc
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            // ORIG：getCurCharacNo() 先于 cMyTrace ctor 求值（临时对象直接调用形态）
            cMyTrace(__PRETTY_FUNCTION__, 0xc472, 0)(
                "Trace Auction Delay, %s(%d)", "REQ AuctionRegistItem",
                user->getCurCharacNo());
            break;
        }
    }

    return 0;
}