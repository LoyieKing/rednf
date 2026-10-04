// ============================================================================
// Arad_INTER_kAvatarRoulette —— ORIG 0x08198a9a
// ============================================================================
int Arad_INTER_kAvatarRoulette::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] 封 N82/P211/P77（ORIG 0x08198a9a）：320 vs 322，残差 2 条 +
    // 全帧槽基 +4（属 N82/N95 槽复用/槽基差，指令一一对应）：
    // ①levelParam unsigned short 局部：4.4 恒 16 位 store+movzwl 回读 vs ORIG
    //   32 位槽直存（P77/P10 跨版本局部宽度；int/unsigned int 承载、去 cast 各试均劣）；
    // ②`picked.m_itemId`（unsigned int 字段→int 形参）4.4 多 1 条 regmove
    //   （ORIG 单条 mov -0x50(%ebp),%ebx；无 cast 形同，P211/N18a）。
    // ③AvatarCoin::Use 已由 Main 代落 bool（原 unsigned int ⇒ 调用点 setne 多 1 条，
    //   P292；323→322）。p167 47/47 CLEAR。
    // 真 bug 修复：①getCurCharacInvenRefW() 返回 CInventory*（巨类），裸 `+0x658`
    // 按元素缩放成 +0x30f740；ORIG 0x08198ca2 是字节偏移 add $0x658 ⇒ (char*) 化。
    // ②G_CEnvironment()+0x1b0 裸偏移改具名 m_serverEnvir.m_channelNo（ORIG 单条
    // mov 0x1b0(%eax),%eax）。③sig 改 arad::SigAvatarRoulette 具名成员。
    if (user == 0)
        return 0x3c8;

    arad::SigAvatarRoulette* sig = (arad::SigAvatarRoulette*)body;  // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3cd, "AVATAR ROULETTE INTER_DSP(SigAvatarRoulette) null.");
        return 0x3ce;
    }
    if (user->getCurCharacR() == 0)
        return 0x3d4;

    char dateKey[24];                                       // -0x44
    memset(dateKey, 0, sizeof(dateKey));                    // ORIG memset(buf,0,0x18)

    time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();   // -0x48
    tm* broken = localtime(&now);                           // -0x28

    OS_API::snprintf(dateKey, 0x14, "M%02d%02d%02d%02d%10d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (broken->tm_year + 1900) % 2000, broken->tm_mon + 1,
                     broken->tm_mday, (unsigned int)user->getCurCharacNo());

    AvatarRouletteServer::PickedAvatarInfo picked;
    // ORIG 0x08198bf3 直接 `mov <A>,%eax` 传 receiver（无 roulette 具名槽）。
    if (!((AvatarRouletteServer*)GlobalData::s_pAvatarRoulette)->Pickup(
            picked, sig->m_type, sig->m_slotIdx))
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3e7, "[kAvatarRoulette] Avatar Roulette Pickup failed. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x236, 1);
        return 0;
    }

    // gradeBand>1 -> 归约成档位 3；否则 -1（ORIG cmpb $0x1,%al; jle = signed char）。
    signed char decBand = (signed char)-1;                  // -0x21
    if (picked.m_gradeBand > 1)
        decBand = (signed char)3;

    // AvatarCoin 可用性判定(refW+0x658)，不可用则拒抽。
    // P48：ORIG 每次使用均重调 getCurCharacInvenRefW()+0x658，不许提升到局部。
    // [b28d-solo] 真 bug 修复：getCurCharacInvenRefW() 返回 CInventory*（巨类），
    // 裸 `+0x658` 按元素缩放成 +0x30f740；ORIG 0x08198ca2 为字节偏移 add $0x658。
    if (!user->getCurCharacInvenRefW()->m_avatarCoin.IsUsable())
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x3f9, "[kAvatarRoulette] need avatarCoin. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return 0;
    }

    int avatarUid = 0;                                      // -0x20
    if ((unsigned char)picked.m_zoneType == 3)
        avatarUid = 0;                                      // ORIG 重写同值，保留指令序
    // [M3S4 落地 · DIFF→AE，320=320] ORIG 0x08198d3d-0x08198d46 `movzbl -0x4b; cbtw;
    // movzwl %ax,%eax; mov %eax,-0x6c` = 32 位槽直存、无 16 位中继槽。旧形
    // `unsigned short levelParam` 在 4.4 下多一个 -0x1e 16 位中继槽（`mov %ax,-0x1e;
    // movzwl -0x1e,%eax; mov %eax,-0x6c`）⇒ 全帧槽基 +4（buf/sig/now/broken/picked/
    // packet 全体下移 4，decBand/avatarUid -0x21/-0x20→-0x25/-0x24）。
    // 实测：①`unsigned int` 32 位承载（`(unsigned short)` 截断）⇒ 323（-0x20 中继槽）；
    // ②删具名变量、把 `(unsigned short)(signed char)picked.m_pad9` **内联进实参位**
    // ⇒ 320，槽集合与 ORIG 全等（-0x1c/-0x20/-0x21/-0x28/-0x2c/-0x44/-0x48..-0x4c/
    // -0x50/-0x54/-0x60/-0x6c/-0x70），TU 四数 diff 2→1、ae 20→21。
    // ORIG 0x08198d57 先装实参到 %edi/%esi/%ebx 再 call getCurCharacInvenW（无槽）。
    int slot = user->getCurCharacInvenW()->AddAvatarItem(
        picked.m_itemId, avatarUid, false,
        (char)picked.m_optionOrSkillIdx, (char)decBand,
        dateKey, (eAvatarItemAddReason)0x15, false,
        (unsigned short)(signed char)picked.m_pad9);

    if (slot == -1)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x407, "[kAvatarRoulette] no space in inventory. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 4);
        return 0;
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1, slot);

    if (!user->getCurCharacInvenRefW()->m_avatarCoin.Use())
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x410, "[kAvatarRoulette] need avatarCoin. (%u - %u)",
                              (unsigned int)user->get_acc_id(),
                              (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 0x16);
        return 0;
    }

    AvatarCoin::HistoryLog::SubLog(user, 1);
    AvatarCoin::SendSyncPacket(user);

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24a);
    packet.put_byte(1);
    packet.put_short((int)(short)slot);
    packet.put_byte((int)(signed char)picked.m_optionOrSkillIdx);
    packet.put_short((int)(signed char)picked.m_pad9);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}