// ---------------------------------------------------------------------------
// CInGameAdvertisementManager::CheckFailMultiBoxLottery —— ORIG 0x080fcb3e
// _ZN27CInGameAdvertisementManager24CheckFailMultiBoxLotteryEP5CUserRSt6vectorISt4pairI10Inven_ItembESaIS5_EE
// 报告 docs/class_func_reports/CInGameAdvertisementManager/CheckFailMultiBoxLottery.md：
//   若 rewardList 全部为 multibox 广告箱物品(idx ∈ {0x1e3c,0x1e3d,0x1e3e})，
//   inc_multiboxLotteryItemFailCnt；连续失败 >2 时发送 put_header(0,0x1c6) 空通知包；
//   任一非广告物品 → set_multiboxLotteryItemFailCnt(0)。单例 owner=GameMain.cpp。
// 定义置于本 TU 末尾（曾试 P210 后置以翻 multibox 帧：实测帧 0x24c 不变，
//   故非「已见定义 callee」成因；见 multibox_lottery_item 判词 N95）。
// ---------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78/N82/N95，gap=-2/blocks=13）→ 试=①prett/q3_gap2
//   缺口 0、strall CLEAR、p167 16/16 CLEAR（E04 已证）②残余两块逐一定性：
//   (a) `Inven_Item item = (*it).first;` 隐式拷贝尾单元宽度：ORIG 4.1 `movzwl 0x3c(%eax),%eax;
//       mov %ax,-0x2a(%ebp)`（word、含 +0x3d 越界读遗留）vs 4.4 `movzbl …/%al`（byte）
//       ⇒ 由 **Inven_Item 头布局最后成员宽度**决定（P344 拷贝单元分段轴），改须动共享头 ⇒ 越界；
//   (b) it/guard 槽序反向（ORIG guard -0x28/it -0x1c vs 我方 it -0x28/guard -0x24）⇒ N95 槽序族。
//   ⇒ 维持 N（147/149，gap −2）。
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
}