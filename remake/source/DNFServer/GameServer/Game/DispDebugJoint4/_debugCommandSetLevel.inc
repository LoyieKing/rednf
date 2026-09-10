char DisPatcher_DebugCommand::_debugCommandSetLevel(CUser* user, int level)
{
    // b28d-37 [N18a 封顶，ORIG 0x0858efde]：`mov $I,%eax` vs `mov $I,%ebx`
    // callee-saved 分配差（264/266），语义闭合。
    if (!user->isGMUser())                       // +0x00e isGMUser; !=1 → ret 0
        return 0;
    if (level > 0x46)                            // +0x024 cmpl $0x46; jle 继续
        return 0;

    bool changed = true;                         // -0x19 local_1d
    int delta = level - user->get_charac_level();  // -0x18 local_1c

    if (delta == 0)
    {
        changed = false;
    }
    else if (delta < 1)
    {
        // +0x0bb 降级环：for(i=0; delta<i; --i) SetCharacLevel(min(200, lv-1))
        for (int i = 0; delta < i; --i)
        {
            int next = std::min(200, user->get_charac_level() - 1);
            user->SetCharacLevel(next);
        }
    }
    else
    {
        // +0x064 升级环：for(i=0; i<delta; ++i) SetCharacLevel(min(lv+1, 200))
        for (int i = 0; i < delta; ++i)
        {
            int next = std::min(user->get_charac_level() + 1, 200);
            user->SetCharacLevel(next);
        }
    }

    user->_onLevelUp();                          // +0x110
    // +0x11b setCurCharacExp(G_CDataManager()->get_level_exp(get_charac_level()))
    int curLevel = user->get_charac_level();
    user->setCurCharacExp(G_CDataManager()->get_level_exp(curLevel));

    if (!changed)                                // +0x14e test local_1d
        return 0;

    user->CalcurateUserMaxLevel();               // +0x158

    // +0x163 雇佣兵信息包：header(1,0x1ba)/byte(1)/SetMercenaryInfoPacket/
    // finalize(true)/Send
    PacketGuard mercenaryPacket;                 // -0x38 local_3c
    mercenaryPacket.put_header(1, 0x1ba);
    mercenaryPacket.put_byte(1);
    user->SetMercenaryInfoPacket(mercenaryPacket);
    mercenaryPacket.finalize(true);
    user->Send(mercenaryPacket);

    // +0x1d3 满装备等级追平角色等级则清零
    bool resetMaxEquip = false;
    int maxEquipLevel = user->GetCurCharacMaxEquipLevel();
    if (maxEquipLevel != 0 &&
        maxEquipLevel == user->get_charac_level())
    {
        resetMaxEquip = true;
    }
    if (resetMaxEquip)
        user->SetCurCharacMaxEquipLevel(0);      // +0x21c
    // ORIG 两分支各持独立 PacketGuard 槽（-0x44/-0x50）；C++ 分支内构造无法
    // 复刻帧布局，提升为函数级共享槽（本地缓冲，无行为差异）。
    PacketGuard questPacket;

    int state = user->get_state();
    if (state == 3)
    {
        // +0x23f 地下城内：0x25 提示 + 双段任务信息刷新
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(0x25), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                             static_cast<ENUM_NOTIPACKET>(2), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
        UserQuest* quest = user->getCurCharacQuestR();
        quest->get_quest_info(reinterpret_cast<char*>(&questPacket));
        user->Send(questPacket);
    }
    else if (state == 5)
    {
        // +0x309 战场内：0x25 提示 + 任务信息刷新
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_NOTIPACKET>(0x25), 0);
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                             static_cast<ENUM_NOTIPACKET>(2), 1);
        UserQuest* quest = user->getCurCharacQuestR();
        quest->get_quest_info(reinterpret_cast<char*>(&questPacket));
        user->Send(questPacket);
    }
    return changed ? 1 : 0;                      // +0x3d0 movzbl local_1d
