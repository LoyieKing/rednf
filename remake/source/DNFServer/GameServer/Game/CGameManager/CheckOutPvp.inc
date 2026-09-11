void CGameManager::CheckOutPvp(CUser* user, bool flag)
{
    // b28d-lane12: N82 槽平移封顶（orig=210 our=210 gap=0；ndiff 270 全为槽号——
    // PacketGuard/CSwitchLog/leave_room 局部槽组）。ORIG 0x08297eaa。
    // b28d-laneCGM 真语义修正（判据①·条件极性）：ORIG 0x08297ec9 `jne 08298197`
    // 落点 = +0x2ed（尾部 nop→epilogue），且报告 Ghidra C 为 `if (cVar2 == '\x01')`
    // ⇒ ORIG 是 `CheckInPvp()` 为**真**时执行 body。旧形 `!user->CheckInPvp()`
    // （我方 1fe0 `je 22be` 同样落尾部 ⇒ 假值进 body）方向相反——真 bug。
    // ORIG 的 `xor $1; test; jne tail` 是 4.1 对 bool 取反的跳转形（保留 xor 段）。
    // b28d-laneCGM：空 then + else 结构复现 ORIG 的 `xor $1,%eax; test %al,%al;
    //   jne <tail>`（P223 变体 / 同 CheckOutTrade 形）；`if(f()){body}` 反向。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] 语义：ORIG 0x08297ec4..0x08297ec9
    //   `call CheckInPvp; xor $1,%eax; test %al,%al; jne <tail>` ⇒ 真值执行 body ✓
    //   现形一致；logFormat 串已按 ORIG 0x082980e3 实字节（.rodata 0x8c16f94 EUC-KR
    //   `BE F8 B4 C2 C0 AF C0 FA PVPOut C3 B3 B8 AE ID[%d] ACCID[%s]`）修正。
    //   DIFF 212/211 残差（读 fresh .o 实测）：① ORIG 0x08298125..0x0829812c
    //   `test %eax,%eax; setg %al; test %al,%al; je`（4.1 的 `>0` 在 && 链 bool
    //   物化）vs 4.4 直产 `jle`（-2 条，P168a/N122 族）；② ORIG 尾 `jmp <epilogue>`
    //   （+1 条）。槽组 -0x1c/-0x20/-0x24/-0x54 ↔ -0x20/-0x24/-0x1c = N82。
    if (!user->CheckInPvp())
    {
    }
    else
    {
        PvP_Room* room = GetPvp((int)user->GetPvpIndex(), user, 0);
        if (room != 0)
        {
            PacketGuard guard;
            bool outFlag = false;
            int ret = room->leave_room(user, outFlag);
            int outFlagInt = outFlag;
            // b28d-laneCGM（P227 全表达式临时链）：ORIG 0x08297f49..0x08297fc3 在
            //   CSwitchLog ctor **之前**右→左求值 battleMode/roomIdx/characName/accName
            //   并以 %edi/%esi/%ebx 跨 ctor 保活；具名局部恒落槽（我方旧形 -0x14/-0x10/-0xc）。
            CSwitchLog(__PRETTY_FUNCTION__, 0x11a1, 0, 0)(
                "pvp@log %s,%s, room(%d),%d,%d",
                user->get_acc_name(), user->getCurCharacName(),
                room->get_index(), room->get_pvp_battle_mode(), outFlagInt);
            // b28d-laneCGM: 反转 if/else 使 4.4 物理块序与 ORIG 一致——ORIG
            //   0x08297fcc `js +0x209` 且 >=0 块直落（+0x128），logFormat 块置后
            //   （+0x209，尾直落 join）；4.4 对 `if(ret<0){A} else {B}` 恒产
            //   `jns B; A; jmp join; B:`（A 前置）⇒ 改写为 `if(ret>=0){B} else {A}`。
            if (ret >= 0)
            {
                guard.clear();
                room->make_seat_info((char*)&guard, ret);
                G_GameWorld()->send_all(guard);
                if (outFlag)
                {
                    guard.clear();
                    room->make_state_info((char*)&guard);
                    G_GameWorld()->send_all(guard);
                }
                if (room->get_waiter_count() == 0)
                {
                    room->destroy_room(0);
                    PutPvp(room);
                    guard.clear();
                    room->make_state_info((char*)&guard);
                    G_GameWorld()->send_all(guard);
                }
            }
            else
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x11bd,
                                      // §6.5 串盲区修正：ORIG 0x082980e3 直证 .rodata
                                      // 0x8c16f94 实字节 = EUC-KR（AE 归一只看地址，看不出）。
                                      "\xbe\xf8\xb4\xc2\xc0\xaf\xc0\xfa PVPOut\xc3\xb3\xb8\xae ID[%d] ACCID[%s]",
                                      room->get_index(),
                                      NumberToString(user->get_acc_id(), 0));
            }
            if (!flag && room->get_recv_pvp_rank_count() > 0)
            {
                if (room->IsInsertTimerRecvPvpRank() != 1 &&
                    room->IsEndPvpBattle() != 1)
                {
                    room->send_pvp_end();
                }
            }
        }
    }
}