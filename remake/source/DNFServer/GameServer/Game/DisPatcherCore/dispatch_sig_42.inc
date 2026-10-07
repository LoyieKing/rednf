// §6.2 巨件核销：P167 117/117 [0缺0多]；gap 761→761/777；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82/setcc-fold：帧 0x8c vs 0x90 + bool 局部槽物化（movb -0x1b）vs ORIG 寄存器岛；语义集合全等  (ORIG 0x081fc4de insn 761->777)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 982（ORIG 756 / ours 772），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_DebugCommand::dispatch_sig(CUser* user, PacketBuf& packet)

{
    MSG_DEBUG_COMMAND* msg =
        reinterpret_cast<MSG_DEBUG_COMMAND*>(packet.get_packet(0));
    // [K6-DpcSigBig L5] P171/P140：ORIG 0x081fc4fd `mov %eax,-0x20(%ebp)` 后
    // **直接** `mov -0x20(%ebp),%eax; movzbl 0xd(%eax),%eax`（0x081fc500..503）
    // ⇒ 原码无判空；旧形发明的 `if (msg == 0) return 0;` 多发 4 条
    // （`cmpl $0,-0x20; jne; mov $0x0,%eax; jmp`）。越界行为照抄 ORIG。

    bool allowed;                                        // ORIG bVar2
    if (msg->m_command != 0)
        allowed = (bool)(unsigned char)GlobalData::s_GM_Manager->GetCurrentGmMode(
            user->get_acc_id());
    else
        allowed = true;
    if (!allowed)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8b6a, 5);
        trace("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b6b, __PRETTY_FUNCTION__, 0, 0);
    }

    cMyTrace trace1(__PRETTY_FUNCTION__, 0x8b65, 0);
    trace1("GM(%d) trying to command(%d)", user->get_acc_id(),
           (int)msg->m_command);

    if ((int)(unsigned int)(size_t)user->getCurCharacR() == 0)
        return LineFunc(0x8b70, __PRETTY_FUNCTION__, 0, 0);

    cMyTrace trace2(__PRETTY_FUNCTION__, 0x8b72, 0);
    trace2("GM Command : %s", NumberToString(user->get_acc_id(), 0));

    bool result = true;                                  // ORIG local_1e(-0x1a)
    bool passFlag = false;                               // ORIG local_1d(-0x19) 恒 0
    // ORIG 反汇编：`IsGm(accId)` 后 xor $1/test；非 GM 走 Illegal trace+
    // LineFunc(0x8b91)（不静默 return 0）；GM 再检 passFlag（movzbl -0x19/
    // xor $1/test，非折叠），假则 return 0、真则 GM Pass trace + switch。
    if ((unsigned char)GlobalData::s_GM_Manager->IsGm(
            user->get_acc_id()) != 1)
    {
        cMyTrace trace3(__PRETTY_FUNCTION__, 0x8b90, 5);
        trace3("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b91, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!passFlag)
    {
        return 0;
    }
    {
        cMyTrace trace4(__PRETTY_FUNCTION__, 0x8bb6, 0);
        trace4("GM Pass Complete! Command(%d)", (int)msg->m_command);
    }

    // ORIG 以 byte 口径取载荷首 int（DisPatcherCore.h:716 MSG_DEBUG_COMMAND
    // `int m_arg` @+0x0e）的第 0/1 字节（movzbl 0xe / 0xf）——取具名成员的
    // 同宽度字节视图，不用裸偏移。
    // [Q5BigTURest.DisPatcherCore 2026-09-11 ③a-2 复查·负结论] 本处是 `disp2` 报
    // ORIG-only `0xe/0xf/0x12` 的根因：ORIG 为**同类型基址 + 常量 disp** 单条读
    // （`mov -0x20(%ebp),%eax; movzbl 0xe(%eax)`，见 ORIG 0x081fca5e/0x081fca68 与
    // 0x081fcab5/0x081fcabf）；我方经具名 `argBytes` 指针 ⇒ 索引退化
    // `mov -0x18(%ebp),%eax; add $0x1,%eax; movzbl (%eax)`。**三试形在 4.4.6 全部不可造**
    // （实测：① 具名 `argBytes=&msg->m_arg` + 索引 0/1；② 立即 cast 表达式
    // `reinterpret_cast<const unsigned char*>(msg)[0xe]`；③ 未偏置具名字节指针
    // `argBytes=(const unsigned char*)msg` + 索引 0xe/0xf —— 三者均产
    // `mov -0xNN(%ebp),%eax; add $0xe,%eax; movzbl (%eax)`，**恒不折 disp**）⇒ 属
    // 4.4 对「异类型 cast 视图」物化基址的固有行为（与 Dispatchers A 组同族天花板）。
    // 正解只能来自共享头：`MSG_DEBUG_COMMAND` 自身在 +0x0e 提供 byte 成员（同类型访问
    // 才折 disp）；属共享头话题，此处保持原形（语义等价，仅 3 条指令形态差）。
    const unsigned char* argBytes =
        reinterpret_cast<const unsigned char*>(&msg->m_arg);   // +0x0e..0x11

    switch (msg->m_command)
    {
    case 0:
    {
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) < GM_LV1)
            return LineFunc(0x8bcb, __PRETTY_FUNCTION__, 0, 0);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        if ((unsigned char)GlobalData::s_GM_Manager->GetCurrentGmMode(
                user->get_acc_id()) == 1)
        {
            user->SetGameMasterMode(true);
            result = _debugCommandOn(user, *msg);
        }
        else
        {
            cMyTrace trace5(__PRETTY_FUNCTION__, 0x8bd0, 0);
            trace5("%d:'%s' GM Mode OFF.", user->get_acc_id(),
                   (const char*)user->getCurCharacName());
            result = true;
            msg->m_command = 0x2a;
            result = _debugCommandOff(user, *msg);
        }
        break;
    }
    case 1:
    case 2:
        result = _debugCommandLevelUpDown(user, -1);
        break;
    case 3:
        result = _debugCommandSetGrowType(user, argBytes[0], argBytes[1]);
        break;
    case 4:
        result = _debugCommandClearSkill(user, *msg);
        break;
    case 5:
        result = _debugCommandSetSkill(user, argBytes[0], argBytes[1]);
        break;
    case 6:
        result = _debugCommandAddItem(
            user,
            (unsigned long)*(const unsigned int*)reinterpret_cast<
                const unsigned char*>(&msg->m_arg),
            *reinterpret_cast<const int*>(&msg->payload));   // +0x12
        break;
    case 7:
        result = _debugCommandSetLevel(user, (int)argBytes[0]);
        break;
    case 8:
        result = _debugCommandPVPRankSet(user, *msg);
        break;
    case 9:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)1);
        break;
    case 0xa:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)2);
        break;
    case 0xb:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)0);
        break;
    case 0xc:
        result = _debugCommandUserRepel(user, *msg);
        break;
    case 0xd:
        G_CGameManager()->WarRoomCountManageTest(msg->m_arg);
        break;
    case 0xe:
        if (!(bool)(unsigned char)
                user->getCurCharacInvenR()->GetCreatureMgrR()
                    ->IsGrowCreature_Equipped_Creature())
            user->getCurCharacInvenW()->GetCreatureMgrW()->GainExp(
                msg->m_arg);
        break;
    case 0xf:
        user->getCurCharacInvenW()->GetCreatureMgrW()->SetStomach(msg->m_arg);
        break;
    case 0x10:
        user->getCurCharacInvenW()->GetCreatureMgrW()->Evolute(0, 0, true);
        break;
    case 0x11:
        result = _debugCommandAddAvatar(user, *msg);
        break;
    case 0x12:
        CBattle_Field::SetEnableHellDungeon(true);
        break;
    case 0x13:
        result = (bool)(unsigned char)GlobalData::s_pGMAccounts_->appendGM(
            user->get_acc_id(), (unsigned int)msg->m_arg);
        break;
    case 0x14:
        // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x081fcd21 `call removeGM; mov %al,-0x1a(%ebp)`，
        //   removeGM（ORIG 0x081093fc）为 `mov $0x0,%eax; leave; ret` ⇒ result=false，
        //   0x081fd047 `cmpb $0,-0x1a; je` 跳过回包。旧写法恒 true 与 ORIG 相反。
        result = (bool)(unsigned char)GlobalData::s_pGMAccounts_->removeGM(
            user->get_acc_id(), (unsigned int)msg->m_arg);
        break;
    case 0x15:
        result = _debugCommandInvisible(user, *msg);
        break;
    case 0x16:
        result = _debugCommandVisible(user, *msg);
        break;
    case 0x17:
        result = _debugCommandRequestMid(user, *msg);
        break;
    case 0x18:
        user->SetCurCharacLuckPoint(msg->m_arg);
        user->enableSaveCharacStat();
        break;
    case 0x19:
        user->SetAge((unsigned int)msg->m_arg);
        break;
    case 0x1a:
        result = _debugCommandSetEnchant(user, *msg);
        break;
    case 0x1b:
        if ((int)(unsigned int)(size_t)user->getCurCharacR() != 0)
            expert_job::CExpertJob::IncreaseExpertJobExp(user, msg->m_arg);
        break;
    case 0x1c:
        result = _debugCommandSetUserDefine(user, packet);
        break;
    case 0x1d:
        result = _debugCommandGuildLevelUp(user);
        break;
    case 0x1e:
        result = _debugCommandPowerWarStart(user);
        break;
    case 0x1f:
        result = _debugCommandPowerWarEnd(user);
        break;
    case 0x20:
        result = _debugCommandPowerUpOn(user, *msg);
        break;
    case 0x21:
        G_GameWorld()->CancelPowerUp();
        break;
    case 0x22:
        result = _debugCommandUserPowerwarPointUp(user, *msg);
        break;
    case 0x23:
        result = _debugCommandInitUserPowerwarPoint(user, *msg);
        break;
    case 0x24:
        result = _debugCommandPowerwarPointUp(user, *msg);
        break;
    case 0x25:
        result = _debugCommandSetBlackCount(user, *msg);
        break;
    case 0x26:
        result = _debugCommandCargoCapacity(user, *msg);
        break;
    case 0x27:
    {
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendIncreaseDecreasePowerWarPoint(
            (unsigned int)(unsigned short)(size_t)user->get_charac_guildkey(),
            static_cast<unsigned int>(user->getCurCharacNo()), msg->m_arg);
        break;
    }
    case 0x28:
        if (msg->m_arg < 1)
        {
            if (msg->m_arg == 0)
            {
                user->RecoverFatigue(0);
                user->SendFatigue();
            }
        }
        else
        {
            user->FatigueUp(msg->m_arg);
            user->SendFatigue();
        }
        break;
    case 0x29:
        if (0 < msg->m_arg)
        {
            user->getCurCharacInvenW()->UseUpCoin((unsigned int)msg->m_arg);
            user->SendCoin();
        }
        break;
    case 0x2a:
        result = _debugCommandOff(user, *msg);
        break;
    case 0x2b:
        result = _debugCommandClearAvtInven(user, *msg);
        break;
    case 0x2c:
        result = _debugCommandClearInven(user, *msg);
        break;
    case 0x2f:
        if (msg->m_arg != 0)
        {
            CInventory* invW = user->getCurCharacInvenW();
            invW->set_money((unsigned int)msg->m_arg);
            const int curMoney = user->getCurCharacInvenR()->get_money();
            user->m_historyLog.MoneyAdd(curMoney, msg->m_arg,
                                        (eMoneyAddReason)6);
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, 0);
            result = true;
        }
        break;
    case 0x30:
        G_GameWorld()->UpdateMiniCraneSeed();
        break;
    default:
        return LineFunc(0x8da2, __PRETTY_FUNCTION__, 0, 0);
    }

    if (result)
    {
        user->SendCmdPacket(CUser::eSendTarget_1, (ENUM_CMDPACKET)0x41,
                            (int)msg->m_command);
        if (msg->m_command == 0)
        {
            user->ReCalcChattingEmoticon();
            user->SendChattingEmoticon();
        }
    }
    return 0;
}
