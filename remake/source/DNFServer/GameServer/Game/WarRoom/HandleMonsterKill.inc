bool WarRoom::HandleMonsterKill(CUser* user, int monsterIdx, CUser* killer)   // [b28d-Main·P292] int→bool
{
    // [SweepWarRoom] DIFF | ORIG 0x086be1fe | 本轮六修：
    //   ①删 result 局部改早返回（ORIG 结果走 %ebx：086be236/086be2fd mov $0,%ebx、
    //     086be852 mov $1,%ebx；尾部 086be8a7 mov %ebx,%eax）；
    //   ②第一循环三独立守卫 goto 岛（ORIG 086be25f je→nop 岛 086be2ac；
    //     086be265/086be277 je/jne→共享尾 086be2ad；body 尾 086be2aa jmp 086be2ad）；
    //   ③CheckQuestMonster 第 3 实参具名 int（ORIG 086be3af movl $1,-0x10 + 086be3cd
    //     mov -0x10,%edx 回读）；
    //   ④exp 乘算书写序 monster.m_field18 在左（ORIG 086be321 flds 先、086be410 fildl 后）
    //     + 具名 float 局部 (float)(unsigned int)exp（ORIG 086be469 fildll→086be46f fstps，
    //     零扩展 mov $0,%edx 证源为 unsigned 转换）；
    //   ⑤total 段书写序 (float)total 在左（ORIG 086be4f4 fildl total 先、086be500 fildl
    //     m_add 后）；
    //   ⑥删 m_slotsUser[i] 指针缓存（P183 地址重物化；ORIG 086be75f/086be78b/086be79e/
    //     086be7c7 每点重算 slot 地址）。
    //   结果：gap 26→3（our 442 / orig 445），槽号全对齐（-0x3a8/-0x4c/-0x40/-0x3c/
    //   -0x34/-0x30/-0x29/-0x28/-0x24/-0x10/-0x54/-0x3ac/-0x3b8/-0x3bc 逐值一致）。
    //   [SweepWarRoom-2] ⑦CheckQuestMonster 第 3 实参局部 int→ENUM_QUEST_ENEMY_TYPE
    //   同型化并删调用点 cast（P41 变体/P248 档①）⇒ 消掉 4.4 提前升格的多余
    //   `mov -0x10(%ebp),%esi`，实参装载序与 ORIG 逐条一致（086be3af movl $1 存槽
    //   → 086be3cd get_index call 后 mov -0x10,%edx 回读）；切片 diff 行 255→251，
    //   our 442 不变（该条与别处 1 条抵消）。
    //   残余 3 = 4.4 强度归约：gain_exp_sp 累加段 ORIG 11 条（mov/add 寄存器目标）
    //   vs 4.4 8 条（lea 合并 + mem-add）；探针 h1（全局）/h20（实参位）证 4.4 对
    //   局部/形参恒 lea+mem-add、源码不可择 ⇒ N86/N1 封顶。
    map_monster monster;
    PacketGuard packet;
    int count = GetWaiterCount();
    if (count == 0)
        return false;
    int wp = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] == 0)
            goto WP_NOP;
        if ((killer != 0) && (m_slotsUser[i] == killer))
        {
            wp = m_field.HandleWpPerMonster(killer, monsterIdx, 0, monster);
        }
        goto WP_END;
    WP_NOP:
        ;
    WP_END:
        ;
    }
    if (m_field.HandleMonsterKill(monsterIdx, packet, monster, killer,
                                  m_pDungeon) != 1)
        return false;
    m_roleKillCnt[monster.m_roleType] += monster.m_field28;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            bool isKiller = (killer != 0) && (m_slotsUser[i] == killer);
            if (isKiller)
            {
                killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                // [SweepWarRoom P41 变体] ORIG 086be3af `movl $0x1,-0x10` +
                //   086be3cd `mov -0x10(%ebp),%edx`（get_index call 之后才回读）
                //   ⇒ 局部与形参同型（enum）时不提前升格 callee-saved。
                ENUM_QUEST_ENEMY_TYPE enemyType = (ENUM_QUEST_ENEMY_TYPE)1;
                killer->CheckQuestMonster(m_pDungeon->get_index(),
                                          monster.m_mobIndex,
                                          enemyType);
            }
            int exp = (int)(long long)(
                monster.m_field18 *
                (float)m_pDungeon->m_warRoomData.m_9c[monster.m_roleType]);
            if (isKiller)
            {
                exp = (int)(long long)(
                    (float)(unsigned int)exp *
                    G_CDataManager()->m_warAreaCounter.m_monsterExpConst);
            }
            ExpBonusPointResult bonus;
            if (isKiller)
            {
                killer->checkBonusPoint();
                killer->getExpBonusPoint(bonus);
            }
            int total = bonus.m_base + exp;
            exp = (int)((float)total * ((float)bonus.m_add / 100.0f)) + total;
            int upperExp = (m_slotsUser[i])->gainExpAsUpperMember(exp);
            int guildExp = (m_slotsUser[i])->gainGuildSkillExp(exp);
            int fatigueExp = 0;
            if ((m_slotsUser[i])->getCurCharacFatigueGrownUpBuff() != 0)
            {
                fatigueExp = exp;
                // ORIG 086be5bb：实参从 -0x18 槽回读（fatigueExp），非 -0x28(exp)。
                (m_slotsUser[i])->set_charac_fatigue_buf_bonus_exp(fatigueExp);
            }
            int rewardExp = (m_slotsUser[i])->gainPowerWarRewardExp(exp);
            int sp = 0;
            int sfp = 0;
            // ORIG 086be652/086be654：test %al,%al; je → addl（bool 直测，无 xor 物化）；
            // 空 then + else-goto 形复现该极性（`== 0` 会多一条 xor $1）。
            if ((m_slotsUser[i])->gain_exp_sp(
                    upperExp + exp + guildExp + rewardExp + fatigueExp,
                    sp, sfp, (CUser::eExpAddReason)4, monster.m_mobIndex,
                    0) != 0)
            {
            }
            else
            {
                goto NOTI_NOP;
            }
            (m_slotsUser[i])->SendNotiPacket(
                (CUser::eSendTarget)3, (ENUM_NOTIPACKET)2, 1);
            goto NOTI_END;
        NOTI_NOP:
            ;
        NOTI_END:
            ;
        }
    }
    unsigned short killerUid = 0xffff;
    if (killer != 0)
        killerUid = (unsigned short)killer->get_unique_id();
    packet.put_byte(1);
    packet.put_short(killerUid);
    packet.put_short(wp);
    packet.finalize(true);
    SendToRoom(packet);
    if (m_field.IsGridClear() != 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                goto FAT_NOP;
            if (m_slotsUser[i]->m_fatigueGain > 0)
            {
                if (m_slotsUser[i]->getCurCharacInvenR()->GetCreatureMgrR()
                        ->IsGrowCreature_Equipped_Creature() != 1)
                {
                    m_slotsUser[i]->getCurCharacInvenW()->GetCreatureMgrW()->GainExp(
                        m_slotsUser[i]->m_fatigueGain);
                }
            }
            m_slotsUser[i]->m_fatigueGain = 0;
            goto FAT_END;
        FAT_NOP:
            ;
        FAT_END:
            ;
        }
        GenTimerKey((TIMER_MESSAGE)0x45);
        if (m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50)
        {
            m_flag12 = 1;
            ChangeState(WARROOM_STATE_5);
        }
        else
        {
            ChangeState(WARROOM_STATE_4);
        }
    }
    return true;
