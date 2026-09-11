// [b28d-11b] N：帧差 0x10 + PacketGuard 槽位（N82）@0x086cad68
void GameWorld::get_user_info(CUser* user, int targetId, int type)
{
    PacketGuard packet;
    if (type == 2)
    {
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(2);
        user->make_basic_info((char*)&packet, 2);
        packet.finalize(true);
        user->Send(packet);
        return;
    }
    if (targetId == 0xffff)
    {
        return;
    }
    CUser* target = find_from_world((unsigned short)targetId);
    if (target == 0)
    {
        LogManager::logFormat(1, "world.cpp",
            __PRETTY_FUNCTION__, 0x107e,
            "[GameWorld::get_user_info] can't find user in world");
        return;
    }
    // verdict=AE（三条件短路链早退，去具名 hidden 落槽）
    // [b28d-solo-GameWorld] ORIG 0x086cae5a `cmp $3; jne 0x86caeac` + 0x086cae7d
    // `xor $1; test; je 0x86caeac` + 0x086caeb3 `test %al; jne 尾` ⇒ 三条件短路链
    // 汇合到统一 false 块（无具名 bool/无预置槽）；具名 hidden 落槽 -0x9 并使帧 +0x10。
    if (type == 3 &&
        !GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) &&
        GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
    {
        return;
    }
    {
        if (type == 3)
        {
            // ORIG +0x15f：receiver = target（-0xc 槽=find_from_world 结果），非 user
            ((CTitleBook*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe))
                ->sendListOtherUser(user, (ENUM_TITLE_BOOK_INFO_TYPE)2);
        }
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(type);   // ORIG +0x1b4 int 直传无扩展；(char) 仅 make_basic_info(+0x1d5 movsbl)
        if (type != 2)
        {
            packet.put_short(1);
        }
        target->make_basic_info((char*)&packet, (char)type);
        packet.finalize(true);
        user->Send(packet);
    }
}