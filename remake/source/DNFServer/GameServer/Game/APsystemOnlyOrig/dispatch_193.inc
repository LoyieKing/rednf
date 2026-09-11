// ORIG 0x08434718（_ZN32DB_UpdateVendingMachineStatistic8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；*stream>>cnt(uint) 再 >>loop(uint)；cnt!=0 时先 inSert
// (item_id=0,cnt=cnt) 一列；再循环 loop 次：>>itemId(ulong) >>itemCnt(uint)，
// itemId==0 提前返 1，逐列 inSert log_vending_stat(occ_date,channel_no,item_id,
// cnt) values(cast(now() as date),%d,%d,%d)；返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 08434718
bool DB_UpdateVendingMachineStatistic::dispatch(int userIdx, int slot,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int cnt = 0;
    unsigned int loop = 0;
    *stream >> cnt;
    *stream >> loop;
    // [J4-r2] 早返形（ORIG 0x0843471e `test %eax,%eax; jne <body>; mov $0x1,%eax;
    // jmp <epilogue>` ⇒ 原码是 `if (cnt == 0) return 1;` 早返，then 块后置；
    // 4.4 对被 if 包裹的 `if(cnt!=0){body}` 产反向 `je` + 内联 mov$1 块
    // ⇒ 改早返复现 ORIG 块序/极性）。
    // [FL3W8LT 2026-09-10 P329/P0] 具名 `channelNo` 已删：ORIG 无 channelNo 槽，
    // `mov -0x14(%ebp),%ebx`（cnt）后直接 `call G_CEnvironment;
    // call get_channel_no; mov %eax,0x??(%esp)` ⇒ 实参位内联（右侧 varargs
    // 求值序保持），同时消掉我方多出的帧 0x48 vs 0x40。
    if (cnt == 0)
        return 1;
    db->set_query(
        "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
        G_CEnvironment()->get_channel_no(), 0, cnt);
    db->exec(true);
    for (unsigned int i = 0; i < loop; ++i)
    {
        unsigned long itemId = 0;
        unsigned int itemCnt = 0;
        *stream >> itemId;
        *stream >> itemCnt;
        if (itemId == 0)
            break;          // [J4-r2] ORIG 0x0843475a `test %eax,%eax; je <尾 mov$1>`
                            // ⇒ 循环内失败与函数尾 `return 1` 共享出口；写 return 1
                            // 会被 4.4 内联成 `mov $1; jmp` 岛（P214②共享尾）。
        db->set_query(
            "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
            G_CEnvironment()->get_channel_no(), itemId, itemCnt);
        db->exec(true);
    }
    return 1;
}