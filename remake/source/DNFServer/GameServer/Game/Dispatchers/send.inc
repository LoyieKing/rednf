void Dispatcher_StartGame::send(CUser* user, ParamBase& param)
{
    // ORIG 0x813f92a
    struct _P4 { int m0; int m1; };   // +0x04 槽（ORIG mov 0x4(%eax) 寻址模拟）
    // 技巧36：ORIG 只 `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 单槽物化；
    // 我方多一个 ParamBase& p 中转变量 ⇒ 多占 -0x18 并把 PacketGuard 挤到 -0x24
    // （ORIG 是 -0x20），整帧位移。直接对形参取地址即可一并归位。
    _P4& q = *reinterpret_cast<_P4*>(&param);
    if (q.m1 == 0)
    {
        StageControl::gotoStage(user);   // ORIG 0x0812fdd4 静态语义
    }
    PacketGuard guard;
    guard.put_header(1, 0x23c);
    guard.put_byte(1);
    guard.put_binary((char*)&q, 8);              // ORIG `mov -0x14(%ebp),%eax`（q 槽）
    if (q.m1 != 0)
    {
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        user->m_advanceAltar.setCurrentAdvanceAltarData(RidableId::RIDEABLE);  // ORIG 常量 1
        user->set_state((ch_state)0xd);
        guard.put_int(1);
        // b28d-solo-Dispatchers verdict=DIFF [N-P63] ORIG 0x0813f92a orig/ours=117/114
        //   ORIG 循环 = 入口 `jmp <底部条件>`（P283）+ 条件 `setle/test/jne` 物化（P51）
        //   两旋钮组合。实测：goto 尾标签形得入口 jmp 但条件被 4.4 折成 `jle`；
        //   do-while 形得 setcc 物化但无入口 jmp ⇒ 两形不可兼得，按 N-P63/N112 封顶。
        // [b28d-solo-S206] 再试 while 形（P25：while 生前置 jmp 条件 + 尾部
        //   cmpl/setle/test/jne 物化；do-while 形缺入口 jmp，goto 形丢 setcc）。
        int i = 0;
        while (i <= 0)
        {
            AdvanceAltarData* data = user->m_advanceAltar.getAdvanceAltarDataByIndex(i);
            if (data != 0)
            {
                putRidableInfoToPacket(guard, *data);
                // [b28d-solo-Dispatchers] P223 形：ORIG 0x0813fa57 `je 0813fa6d`
                //   （跳过清零块）＋ 0x0813fa6a 体尾 `jmp 0813fa6d` ＋ 0x0813fa6c
                //   `nop` 岛 ⇒ 负条件 goto 尾空标签（普通 if 形被 4.4 jump-thread）。
                if (data->m_field20 == 0) goto update_done;
                data->m_field20 = 0;
                data->m_update = 1;      // ORIG +0xcc
                update_done:;
            }
            ++i;
        }
        guard.finalize(true);
        user->Send(guard);
    }
