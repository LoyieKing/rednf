void CInGameAdvertisementManager::CheckFailMultiBoxLottery(
    CUser* user,
    std::vector<std::pair<Inven_Item, bool> >& rewardList)
{
    // b28d-solo-W8_CU_A：按报告 0x080fcb3e 复原（原 b28d-35「N82 帧/槽 + bool
    //   物化块序」判词作废）。三处修法：① 循环体内补具名 `Inven_Item item =
    //   (*it).first;`（ORIG 0x080fcb81 段 15×dword+1×word 内联拷贝到 -0x66 槽，
    //   只读 m_addInfo 不产生该拷贝）；② 白名单判定改正极性
    //   `if (id==A||id==B||id==C) { allAdItems=true; } else { allAdItems=false;
    //   break; }`（ORIG 0x080fcc0b 三 je 汇同一 true 块 + 尾 jne false 块）；
    //   ③ `int failCnt = inc_...();` 具名槽（ORIG 0x080fcc92 `mov %eax,-0xc;
    //   cmpl $0x2,-0xc`）。
    // 残余 N 封顶：① Inven_Item 拷贝尾 4.1 用 word（`movzwl 0x3c; mov %ax`，
    //   越界读 +0x3d，同 CParty.cpp:5915 已证 ORIG 遗留）vs 4.4 byte ⇒ N78；
    //   [E04 2026-09-10 复核] p167 16/16 CLEAR（差集仅 PacketGuard C1/D1↔C2/D2 别名）
    //   ⇒ 无真缺口，gap +1 维持 N78+N95。
    //   ② it/guard 槽序反向（4.1 guard -0x28/it -0x1c，4.4 it -0x28/guard
    //   -0x24）⇒ N95 槽序族（4.4 按首次使用序分配，it 先于 guard 使用）。
    //   本轮已用循环体内 `char slotPad1[1]` 把 item 槽基从 -0x65 顶到 ORIG 的
    //   -0x66（N95 正例，拷贝段 15×dword 偏移全对齐）。语义/调用点
    //   （16/16 CLEAR）与块布局已全等。
    if (user == 0) {
        return;
    }
    if (rewardList.empty()) {
        return;
    }

    // ORIG：迭代器 for 循环（begin/end/++/de）
    bool allAdItems = false;
    for (std::vector<std::pair<Inven_Item, bool> >::iterator it =
             rewardList.begin(); it != rewardList.end(); it++) {
        char slotPad1[1];          // N95 试验：把 item 槽基顶到 ORIG 的 -0x66
        (void)slotPad1;
        Inven_Item item = (*it).first;
        if (item.m_addInfo == 0x1e3cu || item.m_addInfo == 0x1e3du ||
            item.m_addInfo == 0x1e3eu) {
            allAdItems = true;                               // ORIG +0xcd 置一
        } else {
            allAdItems = false;                              // ORIG +0xf1 置零后跳出
            break;
        }
    }

    if (!allAdItems) {
        user->set_multiboxLotteryItemFailCnt(0);             // 0x080fdf44
        return;
    }
    int failCnt = user->inc_multiboxLotteryItemFailCnt();    // 0x080fdf56
    if (failCnt <= 2) {
        return;
    }

    PacketGuard out;
    out.clear();                       // ORIG PacketGuardC1 后显式 clear
    out.put_header(0, 0x1c6);
    out.finalize(true);
    user->Send(out);
