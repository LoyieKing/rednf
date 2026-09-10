int Dispatcher_CreatePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820271c: state==3 且 getCurCharacR()!=0 才放行。
    //   直证：[15] `jne` / [20] `je` ⇒ 取反早退形（旧形 if/else 使 4.4 分支方向相反）。
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(0x98f1, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x066: IsIntegratedPvPBaseChannel || GetChannelType()==7 → error 0x3e
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() || G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x0c8: getUserVillage 空则 LineFunc
    Village* village = G_GameWorld()->getUserVillage(user);
    if (village == 0)
    {
        return LineFunc(0x9905, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x10d: get_area(false) == get_gate_area() → error 0x3e
    if (user->get_area(false) == village->get_gate_area())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x16c: isRestricted(user,1,0xb) → error 0xd1
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0xb))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0xd1);
        return 0;
    }

    // +0x1a1: CSecu_ProtectionField::Check(user,0x3c)
    int secuCheck = GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x3c));
    if (secuCheck != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58),
                                 static_cast<unsigned char>(secuCheck));
        return 0;
    }

    // +0x1ed: 读包五参
    // ORIG [117] 段：`lea -0x21/-0x22/-0x24/-0x26/-0x2e(%ebp)` 直取址、无 `movb/movw $0x0`
    //   预清 ⇒ 五个读包局部均为裸局部（P328⑤）。
    private_store::ISeller* seller = 0;
    unsigned char villageNo;
    unsigned char areaNo;
    short posX;
    short posY;
    unsigned short dollSlot;
    if (!packet.get_byte(villageNo))
    {
        return LineFunc(0x9944, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(areaNo))
    {
        return LineFunc(0x9945, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(posX))
    {
        return LineFunc(0x9946, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(posY))
    {
        return LineFunc(0x9947, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(dollSlot))
    {
        return LineFunc(0x994b, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x33d: 角色村庄/区域与包内一致才放行
    if (static_cast<int>(user->getCurCharacVill()) == static_cast<unsigned int>(villageNo) &&
        static_cast<unsigned int>(user->get_area(false)) == static_cast<unsigned int>(areaNo))
    {
        // 一致 → 创建
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x3a3: GetStoreSeller → 虚 CreatePrivateStore(villageNo,areaNo,posX,posY,dollSlot)
    // ORIG：GetInstancePrivateStoreMgr 结果经 %eax 直传（无 storeMgr 具名指针槽）。
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    if (!seller->CreatePrivateStore(villageNo, areaNo, posX, posY, dollSlot))
    {
        // +0x41a: CreatePrivateStore 失败但状态已建立 → hack 计数
        if (reinterpret_cast<private_store::CPrivateStore*>(seller)->GetState() != 0)
        {
            // ORIG：getHackAnalyzer() 结果经 %eax 直传（无 hackAnalyzer 具名指针槽）。
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x32d), 1, 0, 0);
        }
        // +0x45d: 状态已建立 → 返回；否则释放 seller
        if (reinterpret_cast<private_store::CPrivateStore*>(seller)->GetState() != 0)
        {
            return 0;
        }
        private_store::GetInstancePrivateStoreMgr()->FreeStoreSeller(user);
    }
    return 0;
