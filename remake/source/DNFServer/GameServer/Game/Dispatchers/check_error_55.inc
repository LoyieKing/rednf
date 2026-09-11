int Dispatcher_PartyTeleportConfirm::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081dc75c：user 空 → -1；state!=3 → 0x13；
    // PVP/SkillTree 渠道 → 0x13；无队伍 → 0；
    // 传送状态在 (-∞,-2] 或 [4,∞)（即 state<-1 || state>3）→ 0x13；否则 0。
    if (user == 0)
    {
        return 0xffffffff;
    }
    // ORIG 0x081dc772：user guard 后 msg 指针死物化@-0x10（原码存在未使用的 msg 绑定；
    // P31 dead local，仅复刻 mov/mov 两行）。
    const MSG_BASE* __orig_bind = &msg;
    (void)__orig_bind;
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    // ORIG 两次 G_GameWorld() 调用（0x080da3a7），`||` 展开为布尔合并（mov $1/$0 + test）。
    if (G_GameWorld()->IsPVPChannel() ||
        G_GameWorld()->IsPvPSkilTreeChannel())
    {
        return 0x13;
    }

    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
    {
        // ORIG：`&&` 展开（jge/jle 直跳 + mov $1/$0 + test）；链式调用不缓存 tp。
        if (((CPartyTelePort*)party->GetPartyTelePort())
                    ->get_teleport_state() < -1 &&
            ((CPartyTelePort*)party->GetPartyTelePort())
                    ->get_teleport_state() > 3)
        {
            return 0x13;
        }
    }
    return 0;
}