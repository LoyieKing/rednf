int WarRoom::HandleDieCharacer(CUser* user, int killerSlot)
{
    // [SweepWarRoom] DIFF | ORIG 0x086bd87e | 本轮四修后 gap 2→1（our 324 / orig 325）：
    //   ①killerUid short→unsigned short（ORIG 086bd9ec movw $0xffff + movzwl ⇒ P11，
    //     旧形 movswl 符号扩展）；
    //   ②m_slots[killerSlot] 分支反写为正极性 then=取 uid（ORIG 086bd9a1 je→086bd9a9
    //     cMyTrace 块在 else 之后，P222/P272）；
    //   ③m_flag12 分支反写（ORIG 086bda64 je→086bda96 ⇒ then=「!=0 → 0 参数」）；
    //   ④两处 cMyTrace 改全表达式临时链（P221/P227；trace1 槽 -0x28→-0x38 与 ORIG
    //     深→浅方向一致）。
    // [SweepWarRoom-2 本轮三修，our 324→325 = orig 325，gap 0]：
    //   ⑤删 `int fp` 具名局部改内联比较（P0：ORIG 086bdc16 结果直入 %ebx 无槽），
    //     比较极性改 `fp <= penalty`（ORIG `cmp %eax,%ebx; setle`），then=
    //     DecreaseWarPoint(0) 走 fall-through、else=penalty 段在 je 目标
    //     （086bdc47 je→086bdc68），槽 -0xc 循环变量/循环尾全部对齐；
    //   ⑥m_flag10 分支极性反转 `if (m_flag10 != 0) {ReviveAll…} else {GetLiveCount…}`
    //     （ORIG 086bdd2c GetLiveCount 段在函数最末、ReviveAll 直落，P194）；
    //   ⑦循环尾双源 goto 岛（ORIG 086bdc09 je→086bdcc4 nop 岛 + 086bdcc2 jmp
    //     →086bdcc5，P223 多岛细则）⇒ +0x446 nop 岛与 +0x447 addl 复现。
    //   残余 = game_func_report 切片仅 .o 未解析重定位显示差（call 目标/字面量
    //   地址，N124/N132）；指令数 325=325、全部 jcc 目标下标逐位一致。
    if ((GetState() != 3) && (GetState() != 4) && (m_state != 5))
        return 0x13;
    int slot = GetUserSlot(user);
    if (slot < 0)
        return 4;
    if (m_liveFlag[slot] == 0)
        return 0x12;
    if ((killerSlot != 0xff) && ((killerSlot < 0) || (killerSlot > 5)))
    {
        // P221/P227：全表达式临时链复现 ORIG 槽向（先出现者占更深槽 -0x34）。
        cMyTrace(__PRETTY_FUNCTION__, 0x6d0, 5)(
            "WarRoom::HandleDieCharacer killerSlotIndex Error(%d)", killerSlot);
        return 1;
    }
    // ORIG 086bd9ec movw $0xffff,-0xe + 086bda0x movzwl ⇒ unsigned short（P11）。
    unsigned short killerUid = 0xffff;
    if (killerSlot != 0xff)
    {
        // ORIG 086bd9a1 test/je→086bd9a9(cMyTrace 块，位于 else 块之后)：
        // 正极性 then=取 uid、else=cMyTrace 才能复现 4.1 块序（P222/P272）。
        if (m_slots[killerSlot] != 0)
        {
            killerUid = (m_slotsUser[killerSlot])->get_unique_id();
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x6de, 5)(
                "WARAREA WarRoom::HandleDieCharacer player_[] is NULL killerSlotIndex(%d)",
                killerSlot);
            killerUid = -1;
        }
    }
    SetCharacterLive(slot, user, false, false, killerUid, killerSlot);
    if ((killerSlot != 0xff) && (killerSlot >= 0) && (killerSlot <= 5) &&
        (m_slots[killerSlot] != 0))
    {
        // ORIG 086bda64 je→086bda96（else 块在 fall-through 之后）⇒ 源码为
        // `if (m_flag12 != 0) {0 参数} else {m_d4}`（then 在前）。
        if (m_flag12 != 0)
        {
            (m_slotsUser[killerSlot])->WarAreaKill(4, 0, 1.0f);
        }
        else
        {
            (m_slotsUser[killerSlot])->WarAreaKill(
                4, m_pDungeon->m_warRoomData.m_d4, 1.0f);
        }
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x5f);
        packet.put_byte(1);
        packet.put_byte(killerSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    // [SweepWarRoom P194/P222] ORIG 086bdd2c（GetLiveCount 段）位于函数最末、
    //   ReviveAll 段走 fall-through（086bdb72 之后）⇒ 源码极性为
    //   `if (m_flag10 != 0) {ReviveAll…} else {GetLiveCount…}`（4.4 对正极性
    //   then 恒 fall-through，与 4.1 的块序一致）。
    if (m_flag10 != 0)
    {
        ReviveAll();   // ORIG 0x086bdbb2：m_flag10!=0 时先全员复活
        if ((m_field.m_stageIdx == m_pDungeon->m_warRoomData.m_50) ||
            (m_flag11 != 0))
        {
            return 0;
        }
        PacketGuard packet;
        packet.put_header(0, 0x156);
        packet.put_byte(GetWaiterCount());
        for (int i = 0; i <= 5; ++i)
        {
            // [SweepWarRoom P223] ORIG 086bdc09 `je 086bdcc4`（nop 岛）+
            //   086bdcc2 `jmp 086bdcc5`（体尾岛）⇒ 双源 goto 到连续尾标签。
            if (m_slots[i] == 0)
                goto DIE_NEXT;
            {
                // [SweepWarRoom P0] ORIG 086bdc16..086bdc30：GetFinishPointTotal 结果
                //   直入 %ebx（无槽往返）⇒ 原码无 fp 具名局部；比较为
                //   `cmp %eax,%ebx; setle`（fp<=penalty），then=DecreaseWarPoint(0)
                //   走 fall-through（ORIG 086bdc40 段），else=penalty 段在 je 目标。
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
            goto DIE_NEXT;
        DIE_NEXT:
            ;
        }
        packet.finalize(true);
        SendToRoom(packet);
        m_flag11 = 1;
    }
    else
    {
        if (GetLiveCount() == 0)
            SendWarRoomFailMsg();
    }
    return 0;
