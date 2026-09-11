void GameWorld::check_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                                char* msg, int len)
{
    // ORIG 0x86cab92 段1：`call G_GameWorld; mov %eax,(%esp); call IsPvP` 自由函数形；
    // pvpDiff/black 物化无槽（`test %al` 直判）⇒ 历史 register bool asm("al")；
    // b28d-lane11 按 P0/N96 清绑。
    // verdict=AE（去具名 pvpDiff/black，&&/|| 直判无槽）
    // [b28d-solo-GameWorld] _ZN9GameWorld31check_peerTypemsg_sameWorldUserEP5CUserS1_cPci：
    // ORIG 0x86cab.. `&& 结果 %al; test %al,%al; je`（无槽）⇒ 去具名 pvpDiff/black 内联
    // （具名版多 mov %al,-0x9/-0xa + cmpb 且帧 0x34→0x44）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        (from->GetServerGroup() != to->GetServerGroup()))   // +0x03a mov $1/$0 物化
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x15, 0x15, msg, len);
        return;
    }
    if (to->isBlackUser(from->get_charac_no(-1)) ||        // +0x08e..0x0ab 臂1
        from->isBlackUser(to->get_charac_no(-1)))          // +0x0b4..0x0d1 臂2
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x4d, 0xfe, msg, len);
    }
    else
    {
        if (GlobalData::s_pGMAccounts_->isGM(to->get_acc_id()))     // +0x134 acc_id 先算
        {
            send_peerTypemsg_sameWorldUser(to, from, type, 0x5a, 0x20, msg, len);
        }
        else
        {
            send_peerTypemsg_sameWorldUser(from, to, type, 0, 1, msg, len);
        }
    }
}