void WarRoom::ClearReward()
{
    // [b28d-solo-WarRoom] DIFF(504) | ORIG 0x086bc5fe | 复核 b25 判词：P167
    //   58/58 归零；b25 已补 avatarExp/creatureExp 乘算真 bug；残余为 N82 帧槽
    //   平移 + N76/N18a store 装载序 ⇒ 语义完成、byte 层 N 封顶。
    unsigned int maxRef[2];
    int totalExp;
    int idx1;
    int idx2;
    totalExp = 0;
    for (idx1 = 0; idx1 < 4; ++idx1)
    {
        totalExp += (int)(m_roleKillCnt[idx1] *
                          (float)m_pDungeon->m_warRoomData.m_9c[idx1]);
    }
    for (idx2 = 0; idx2 < 6; ++idx2)
    {
        if (m_slots[idx2] != 0)
        {
            PacketGuard packet1;
            PacketGuard packet2;
            int sp = 0;
            int sfp = 0;
            float rate = m_pDungeon->m_warRoomData.m_ac;
            int fp = (m_slotsUser[idx2])->GetFinishPointTotal();
            float finishPointExp = (float)fp * rate;
            unsigned int baseExp = (unsigned int)(long long)(
                (finishPointExp + 1.0f) * (float)totalExp);
            float avatarRate = ((m_slotsUser[idx2])->IsEquipAvatar() != 0) ? 0.02f : 0.0f;
            float creatureRate =
                ((m_slotsUser[idx2])->getCurCharacInvenR()->GetCreatureMgrR()
                         ->IsEquippedCreature() != 0)
                    ? 0.05f
                    : 0.0f;
            int channelExp = 0;
            int serverExp = 0;
            unsigned int avatarExp = 0;
            int eventExp = 0;
            unsigned int creatureExp = 0;
            int premiumExp = 0;
            float expFactor = 0.0f;
            if (((CExpDoubleEvent*)GlobalData::s_event_manager->GetRepeatEvent(3))
                    ->IsEventing(0))
            {
                expFactor =
                    (float)((CExpDoubleEvent*)GlobalData::s_event_manager->GetRepeatEvent(3))
                        ->GetExpFactor() /
                    100.0f;
            }
            void* channel = G_CDataManager()->GetChannelScript();
            // 渠道组字节：ORIG movzbl G_CEnvironment()+0x378 低字节
            //             = Server_Envir::m_serverGroup(+0x1e0)
            float bonus = ((channel_script_t*)channel)->getBonusRate(
                (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                (unsigned int)G_CEnvironment()->get_channel_no(),
                (unsigned int)m_pDungeon->get_index());
            channelExp = (int)((float)(unsigned long long)baseExp * bonus);
            float serverRate = 0.0f;
            serverRate = (float)CServerEvent::GetExpRate() + serverRate;
            serverRate = serverRate / 100.0f;
            serverExp = (int)(long long)(
                (float)(unsigned long long)baseExp * serverRate);
            // ORIG 0x086bc398/0x086bc461：avatar/creature 奖励先按 baseExp×rate
            // 完整乘算落槽（fildll+fmuls+fistpll），再在 rate!=0 时 max(·,1)。
            // 原重建缺两段乘法赋值（仅剩 max 比较），b25 §6.2 回扫补齐。
            avatarExp = (unsigned int)(long long)(
                (float)(unsigned long long)baseExp * avatarRate);
            if (avatarRate != 0.0)
            {
                maxRef[0] = 1;
                avatarExp = std::max<unsigned int>(avatarExp, maxRef[0]);
            }
            eventExp = (int)(long long)(
                (float)(unsigned long long)baseExp * expFactor);
            creatureExp = (unsigned int)(long long)(
                (float)(unsigned long long)baseExp * creatureRate);
            if (creatureRate != 0.0)
            {
                maxRef[1] = 1;
                creatureExp = std::max<unsigned int>(creatureExp, maxRef[1]);
            }
            if ((m_slotsUser[idx2])->IsHavePremiumAdvantage() != 0)
            {
                float premiumRate = 0.0f;
                premiumRate =
                    (float)((m_slotsUser[idx2])->GetPremiumInfo()->GetAdvantageExpRate()) /
                    100.0f;
                premiumExp = (int)(long long)(
                    (float)(unsigned long long)baseExp * premiumRate);
            }
            int burningBonus = 0;
            CEventBase* burning = GlobalData::s_event_manager->GetRepeatEvent(0xf);
            if (((CBurningFatigueEvent*)burning)->IsEventing(0))
            {
                int rate2 = ((CBurningFatigueEvent*)burning)->getBonusExpRate(
                    (m_slotsUser[idx2])->getCurCharacUsedFatigue());
                if (rate2 != 0)
                    burningBonus = (rate2 * baseExp) / 100;
            }
            int total = avatarExp + baseExp + premiumExp + serverExp + channelExp +
                        burningBonus + creatureExp;
            int upperExp = (m_slotsUser[idx2])->gainExpAsUpperMember(baseExp);
            total += upperExp;
            int guildExp = (m_slotsUser[idx2])->gainGuildSkillExp(baseExp);
            total += guildExp;
            if ((m_slotsUser[idx2])->gain_exp_sp(total, sp, sfp,
                                                  (CUser::eExpAddReason)6, 0, 0) != 0)
            {
                packet2.clear();
                packet2.put_header(0, 2);
                packet2.put_byte(1);
                packet2.put_short(1);
                (m_slotsUser[idx2])->make_basic_info((char*)&packet2, 1);
                packet2.finalize(true);
                (m_slotsUser[idx2])->Send(packet2);
            }
            packet1.clear();
            packet1.put_header(0, 0x5c);
            packet1.put_byte(sp);
            packet1.put_byte(sfp);
            packet1.put_int((m_slotsUser[idx2])->GetFinishPointTotal());
            packet1.put_int(baseExp + serverExp + burningBonus);
            packet1.put_int(avatarExp);
            packet1.put_int(premiumExp);
            packet1.put_int(channelExp);
            packet1.put_int(upperExp);
            packet1.put_int(creatureExp);
            packet1.finalize(true);
            (m_slotsUser[idx2])->Send(packet1);
            int cur = GlobalData::s_systemTime_.getCurSec();
            (m_slotsUser[idx2])->getHackAnalyzer()->setLastMonsterDeadTime(cur);
        }
    }
