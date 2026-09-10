unsigned int DisPatcher_StartGame::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() == 3 || user->get_state() == 0xb)
    {
        // state 合法，继续
    }
    else
    {
        return 0x7fffffff;
    }
    stStartGameParam& paramRef = static_cast<stStartGameParam&>(param);
    int* local_18 = &paramRef.m_8;
    if (G_GameWorld()->IsPVPChannel() != false)
    {
        *local_18 = 0xffffffff;
        return 8;
    }
    if (user->IsBusyDisjointStore() != false)
    {
        *local_18 = 0xffffffff;
        return 0x13;
    }
    if (user->isCompetitionMercenary() != false)
    {
        *local_18 = 0xffffffff;
        return 0xea;
    }
    CParty* pCVar5 = static_cast<CParty*>(user->GetParty());
    if (pCVar5 != 0)
    {
        CPartyTelePort* pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
        char cVar2 = pCVar6->get_teleport_state();
        bool bVar1;
        if (cVar2 < '\0')
        {
            bVar1 = false;
        }
        else
        {
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            cVar2 = pCVar6->get_teleport_state();
            if ('\x03' < cVar2)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = true;
            }
        }
        if (bVar1)
        {
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            pCVar6->reset_teleport_data();
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            pCVar6->send_teleport_status('\0');
        }
    }
    const char* pcVar7 = user->getCurCharacName();
    if (G_GameWorld()->IsForbiddenMove(pcVar7) != false)
    {
        // [b28e-L5] C05④ 步骤2 实测**不命中**（已回退）：ORIG 0x081ca0de 为
        // `test %eax,%eax; setne %al; test %al,%al; je`（body 以 `return 0x44` 终止、
        // 表面符合「含 return」的 P225 命中前件），但整块换 `while (cond != 0)
        // { body; break; }` + 去具名 iVar3 后 our_insns **246→249**（ORIG 228）劣化
        // ⇒ 本件残差主体是 N82 帧差（0x34 vs 0x58）+ 条件极性折痕，保持原形。
        pcVar7 = user->getCurCharacName();
        int iVar3 = G_GameWorld()->GetLeftTimeOfForbiddenMove(pcVar7);
        if (iVar3 != 0)
        {
            *local_18 = 0;
            return 0x44;
        }
        pcVar7 = user->getCurCharacName();
        G_GameWorld()->EnableMove(pcVar7);
    }
    if (user->CheckInParty() != false && user->get_state() == 3)
    {
        pCVar5 = static_cast<CParty*>(user->GetParty());
        if (pCVar5 != 0)
        {
            if (pCVar5->CheckMemberArea() == '\x01' || pCVar5->is_quick_party() == '\x01')
            {
                // 成员同区或快速队伍，继续
            }
            else
            {
                *local_18 = 0xffffffff;
                return 0x15;
            }
            if (excludeAddHackCount(user) != false)
            {
                WongWork::CHackAnalyzer* pCVar8 = user->getHackAnalyzer();
                pCVar8->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x1f6), 1, 0, 0);
            }
            int iVar3b = G_GameWorld()->GetWorldMapIndex(user);
            if (0 < iVar3b)
            {
                CWorldMap* this_00 = G_CDataManager()->m_worldMapList.find_world_map(iVar3b);
                if (this_00 == 0)
                {
                    *local_18 = 0xffffffff;
                    return 0x15;
                }
                if (GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon() != '\x01')
                {
                    int iVar3c = this_00->getDungeonMinLevel();
                    int iVar9 = pCVar5->getMemberMinLevel();
                    if (iVar9 + 6 <= iVar3c)
                    {
                        *local_18 = 0xffffffff;
                        return 0xe;
                    }
                }
            }
            return 0;
        }
    }
    return 0;
