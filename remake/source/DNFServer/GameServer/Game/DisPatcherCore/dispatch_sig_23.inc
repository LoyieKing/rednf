int DisPatcher_DiePvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 7)
    {
        return 0;
    }
    // [FL4 负结论] P232 撑帧法对本件无效：补 `char pad[0x10]` 后 `sub` 立即数虽对齐
    // 0x6c，但全部局部槽相对 ORIG 由「深 4」扩为「深 0x14」（ORIG PacketGuard -0x44 /
    // 我方 -0x58），ndiff 30→28 仅来自帧行本身 ⇒ 判 N82（4.1 按生命周期穿插 vs 4.4
    // 按首次使用分区的槽分配差），保持原帧。
    unsigned short targetIdx = 0xffff;
    if (!packet.get_short(targetIdx))
    {
        return LineFunc(0x6f12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* target = G_GameWorld()->find_from_world(targetIdx);
    if (target == 0)
    {
        targetIdx = 0xffff;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x6f1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard guard;
    int result = room->die_charac(user, target);
    int battleMode = room->get_pvp_battle_mode();   // ORIG -0x4c 具名槽
    // [Lane5] P227/P37 全表达式临时链：ORIG 0x081ecea4..0x081eceb8 在 ctor **之前**
    // 求值 get_index/getCurCharacName/get_acc_name 到 %edi/%esi/%ebx 保活
    // （两语句形会把 ctor 提到 getter 之前）⇒ 构造即调用单表达式。
    CSwitchLog(__PRETTY_FUNCTION__, 0x6f22, 0, 0)(
        "pvp@log %s,%s,room(%d),%d,%d,%d",
        user->get_acc_name(), user->getCurCharacName(), room->get_index(),
        battleMode, result, 0x3a);
    if (0 < result)
    {
        guard.put_header(1, 0x3a);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
