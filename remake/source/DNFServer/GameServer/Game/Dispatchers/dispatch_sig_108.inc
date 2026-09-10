int Dispatcher_BuyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        if (user != 0 && user->CheckInTrade() == 0)
        {
            ServiceRestrictManager* srm = ARAD::Singleton<ServiceRestrictManager>::Get();
            if (srm->isRestricted(user, (RestrictCategory::Enum)1, 0xb))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, 0xd1);
                return 0;
            }
            int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x3d);
            if (secu != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55,
                                         (unsigned char)(secu & 0xff));
                return 0;
            }
            if (user->isHackUser() != 0 || user->isHackUserTimer() != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, 0x11);
                return 0;
            }
            short sellerIndex = 0;
            int count = 0;
            private_store::IBuyer* buyer = 0;
            if (packet.get_short(sellerIndex))
            {
                if (packet.get_int(count))
                {
                    if (count < 0)
                    {
                        return LineFunc(
                            0x98ca,
                            __PRETTY_FUNCTION__,
                            0, 0);
                    }
                    private_store::CPrivateStoreMgr* mgr =
                        private_store::GetInstancePrivateStoreMgr();
                    // ORIG 0x08202543：调 3 参 CUser* 版（直接传 (CUser*)0），
                    // 非 unsigned short 重载（后者会多一次 find_from_world 解析）。
                    mgr->GetStoreBuyer(&buyer, user, (CUser*)0);
                    buyer->BuyPrivateStoreItem((int)sellerIndex, count);
                    return 0;
                }
                return LineFunc(
                    0x98c9,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            return LineFunc(
                0x98c8,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        return 0;
    }
    return LineFunc(
        0x9891,
        __PRETTY_FUNCTION__,
        0, 0);
