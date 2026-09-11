// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08202322 orig/ours=160/162 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x24,%esp | jne    0820234f <+0x2d> | movl   $0x0,0xc(%esp)
int Dispatcher_BuyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820..：[13] `jne` + [19] 段 LineFunc(0x9891) 就地早退（我方旧形正向
    //   if + 函数尾 return LineFunc ⇒ 该岛被沉到尾部、ndiff 69）。
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0x9891,
            __PRETTY_FUNCTION__,
            0, 0);
    }
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
            // ORIG [106]=0x98c8（get_short 失败）在 [120]=0x98c9（get_int 失败）**之前**
            //   ⇒ 原码是三段取反早退，而非嵌套 if（嵌套形令两个 LineFunc 岛相对次序反转）。
            if (!packet.get_short(sellerIndex))
            {
                return LineFunc(
                    0x98c8,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            if (!packet.get_int(count))
            {
                return LineFunc(
                    0x98c9,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            if (count < 0)
            {
                return LineFunc(
                    0x98ca,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            // ORIG 0x08202543：调 3 参 CUser* 版（直接传 (CUser*)0），
            // 非 unsigned short 重载（后者会多一次 find_from_world 解析）；
            // mgr 无具名指针槽（GetInstancePrivateStoreMgr 结果经 %eax 直传）。
            private_store::GetInstancePrivateStoreMgr()
                ->GetStoreBuyer(&buyer, user, (CUser*)0);
            buyer->BuyPrivateStoreItem((int)sellerIndex, count);
            return 0;
        }
        return 0;
    }
}