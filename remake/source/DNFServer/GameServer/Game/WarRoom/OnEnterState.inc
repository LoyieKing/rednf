void WarRoom::OnEnterState()
{
    // [b28d-solo-WarRoom] DIFF(415) | ORIG 0x086bb71c | 复核 b20 判词：P167
    //   68/68 归零；残余为 N82 帧槽平移 + N76/N18a store 装载序 + 4.1 判定树
    //   分派形态（本轮 P235 视角替换后复验无新增残差）⇒ 语义完成、byte 层 N 封顶。
    CancelAllTimer();
    InvalidUserCheck();
    SendStatePacket();
    switch (m_state)
    {
    case 0:
        UpdateBanList();
        if (m_field30 != 0)
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                    WalkOutUserBySlot(i);
            }
            Close();
            m_field30 = 0;
            PacketGuard packet;
            packet.put_header(0, 0x58);
            packet.put_short(1);
            MakeStateInfo((char*)&packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        break;
    case 1:
        m_fpBase = GlobalData::s_systemTime_.getCurSec();
        BattleReset();
        for (int i = 0; i <= 5; ++i)
        {
            // [b28d-solo-WarRoom-3] ORIG 086bb89f `je <else块(movb $0x0)>`：
            // 正极性 then=置 1 先落（P222/P214① 块序）⇒ 源码写 != 0 分支。
            if (m_slots[i] != 0)
                m_liveFlag[i] = 1;
            else
                m_liveFlag[i] = 0;
        }
        if (GetWaiterCount() <= 2)
        {
            bool hasGM = false;
            for (int i = 0; i <= 5; ++i)
            {
                // [b28d-solo-WarRoom-3] ORIG 086bb8ff/086bb918 两处 `je` 直跳
                // 循环增量（086bb920 nop 岛 / 086bb921 addl），无 mov $1/$0 归一岛
                // ⇒ 拆嵌套 if（`&&` 在 4.4 会物化 bool 再 test）；外层失败出口
                // 用负条件 goto 到循环体尾空标签复现 086bb920 nop 岛（P223）。
                if (m_slots[i] == 0)
                    goto GM_LOOP_NEXT;
                if ((m_slotsUser[i])->isGMUser())
                {
                    hasGM = true;
                    break;
                }
GM_LOOP_NEXT:
                ;
            }
            if (!hasGM)
            {
                ChangeState(WARROOM_STATE_0);
                return;
            }
        }
        // [b28d-solo-WarRoom-3] P24/P52：ORIG 086bb966 `call GenTimerKey; mov %eax,%edi`
        // 为内联实参（值保 callee-saved 跨 G_CDataManager/G_TimerQueue），ms 亦为
        // 表达式直传（086bb973 `mov 0x88a8(%eax),%eax; mov %eax,%ebx`）⇒ 禁具名局部。
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x41,
            G_CDataManager()->m_warAreaCounter.m_readyCount,
            GenTimerKey((TIMER_MESSAGE)0x41), 0);
        break;
    case 2:
        // [b28d-solo-WarRoom-3] ORIG 086bb9ce `call GenTimerKey; mov %eax,%ebx` 内联实参。
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)3, m_index, (TIMER_MESSAGE)0x42, 11000,
            GenTimerKey((TIMER_MESSAGE)0x42), 0);
        m_curSpawnStep = 1;
        break;
    // [b28d-solo-WarRoom-3] P324：ORIG 叶块物理序 = case 0,1,2,**4,3**,5
    //（086bba27=ReviveAll 块先于 086bbab3=m_flag10 块）⇒ case 书写序须照 ORIG
    // 块地址序排列，故 case 4 段写在 case 3 段之前。
    case 4:
        ReviveAll();
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x43, 10,
                                        GenTimerKey((TIMER_MESSAGE)0x43), 0);
        if (m_curSpawnStep != m_pDungeon->m_warRoomData.m_50)
            m_curSpawnStep += 1;
        break;
    case 3:
        m_flag10 = 0;
        m_flag11 = 0;
        {
            PacketGuard packet;
            packet.clear();
            // [b28d-solo-WarRoom-3] ORIG 086bbb04 `call GetWaiterCount; mov %eax,%ecx`
            // 无槽往返 ⇒ 原码为内联实参（P0/P24），具名 count 会多占槽。
            m_field.ConsistMap(&packet, m_pDungeon, (const CMap*)m_fp,
                               (unsigned char)(((unsigned char)m_levelBand + 1) * 10),
                               GetWaiterCount());
            SendToRoom(packet);
            for (int i = 0; i <= 5; ++i)
            {
                if (m_slots[i] != 0)
                {
                    // [b28d-solo-WarRoom-3] 字段偏移真 bug 修正：ORIG 086bbb6a 为
                    // `mov 0x7e8(%eax),%edx`（= m_warRoomData(+0x6f4) + 0xf4 = m_f4），
                    // 原重建写 m_ec(+0xec) 读错字段。
                    (m_slotsUser[i])->FatigueUp(
                        m_pDungeon->m_warRoomData.m_f4);
                }
            }
            // [b28d-solo-WarRoom-3] ORIG 086bba40 `call GenTimerKey; mov %eax,%ebx`
            // 与 m_fc/1000 均内联实参（P24）；ORIG 086bbbcd `mov 0x7f0(%eax),%ecx`
            // = +0x6f4 + 0xfc = m_fc（原重建误写 m_f4）。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                            (TIMER_MESSAGE)0x45,
                                            m_pDungeon->m_warRoomData.m_fc / 1000,
                                            GenTimerKey((TIMER_MESSAGE)0x45), 0);
        }
        break;
    case 5:
        if ((m_flag10 != 0) && (m_field.m_stageIdx ==
                                m_pDungeon->m_warRoomData.m_50))
        {
            PacketGuard packet;
            packet.put_header(0, 0x156);
            packet.put_byte(GetWaiterCount());
            for (int i = 0; i <= 5; ++i)
            {
                // [b28d-solo-WarRoom-3] ORIG 086bbcbf `je 086bbd6b(nop 岛)` +
                // body 尾 086bbd69 `jmp 086bbd6c(addl)` ⇒ 双源 goto 到尾空标签
                // （P223 定案），单用 if 包裹会被 4.4 jump-thread 掉 nop 岛。
                if (m_slots[i] == 0)
                    goto SEND_LOOP_NEXT;
                {
                    // [b28d-solo-WarRoom-3] ORIG 086bbcda `call GetFinishPointTotal;
                    // mov %eax,%ebx`（无槽）⇒ 内联实参（P0）。条件与块序照
                    // 086bbce7 `cmp %eax,%ebx; setle %al; test; je 086bbd0f`：
                    // setle 为假(fp>penalty)才跳 then 块 ⇒ 源码须写**否定条件**
                    // 且 else 直落（086bbcf0 为 DecreaseWarPoint(0) 块、
                    // 086bbd0f 为 penalty 块）—— P194/P59 镜像布局。
                    if ((m_slotsUser[i])->GetFinishPointTotal() <=
                        G_CDataManager()->m_warAreaCounter.m_wpPenalty)
                    {
                        (m_slotsUser[i])->DecreaseWarPoint(0);
                    }
                    else
                    {
                        (m_slotsUser[i])->DecreaseWarPoint(
                            G_CDataManager()->m_warAreaCounter.m_wpPenalty);
                    }
                    packet.put_byte(i);
                    packet.put_int((m_slotsUser[i])->GetFinishPointTotal());
                }
SEND_LOOP_NEXT:
                ;
            }
            packet.finalize(true);
            SendToRoom(packet);
        }
        for (int i = 0; i <= 5; ++i)
        {
            // [b28d-solo-WarRoom-3] ORIG 086bbde4 `je 086bbe17(nop 岛)` 与
            // 086bbe02 `je 086bbe18(addl)` 落点不同 + body 尾 086bbe15 `jmp`
            // ⇒ 负条件 goto 尾空标签（P223 定案形，同 hasGM 循环）。
            if (m_slots[i] == 0)
                goto SLOT_ISLAND;
            if ((m_slotsUser[i])->GetFinishPointTotal() == 0)
                m_slotData[i] = 6;
SLOT_ISLAND:
            ;
        }
        ClearReward();
        SendGuildFP();
        RemoveKC();
        // [b28d-solo-WarRoom-3] ORIG 086bbe56 `call GenTimerKey; mov %eax,%ebx` 内联实参。
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)3, m_index,
                                        (TIMER_MESSAGE)0x44, 0x18,
                                        GenTimerKey((TIMER_MESSAGE)0x44), 0);
        break;
    default:
        break;
    }
