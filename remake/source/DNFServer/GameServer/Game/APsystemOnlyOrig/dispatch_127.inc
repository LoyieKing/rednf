// ============================================================================
// DB_ForbidUserToPlay::dispatch —— ORIG 0x08421af4（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_FORBID_USER_TO_PLAY>（视图
// DB_ForbidUserToPlayView：+0x00 characNo、+0x04 flag byte、+0x05 days
// int）。flag==1 且 days!=0 → upDate charac_stat set forbidden_to_play=1,
// forbidden_due_to=date_add(now(), interval %d day) where charac_no=%d
// （days, characNo）；否则 → forbidden_to_play=0, forbidden_due_to=0
// （characNo）。返回 exec(true)==1。
// ============================================================================
// ORIG 帧：sig(-0x1c)/db(-0x18)/characNo(-0x14)/flag(-0xd)/days(-0xc)，
// 三局部先清零再赋值（gcc4.1 形态）；判定 flag==0 → else（清零 SQL）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08421af4
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 5 = OUR 5，0/0）+ gap −1（58/59）
//   ⇒ 语义完成；余 N-JMP label-stub fall-through jmp 封顶。
bool DB_ForbidUserToPlay::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_FORBID_USER_TO_PLAY* sig = /*SIG_FORBID_USER_TO_PLAY*/(
        stream->GetOutBuffer<SIG_FORBID_USER_TO_PLAY>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    int characNo = 0;
    bool flag = false;
    int days = 0;
    characNo = sig->m_characNo;
    flag = sig->m_flag;
    days = sig->m_days;
    // ORIG 0x8421b55-0x8421b64 双 goto 钉块序（技巧23+24）：
    // `movzbl -0xd,%eax; xor $0x1,%eax; test %al,%al; jne b66(1-arg块)` +
    // `cmpl $0,-0xc; jne b82(2-arg块)`；4.4 对 `if(a&&b){}else{}` 直判
    // cmpb$1 融合且块序反（2-arg fall）。fall-through 块 = 1-arg SQL。
    // bool 槽 + `!flag`（f1 探针形）⇒ `movzbl slot,%eax; xor $1; test
    // %al; jne` 物化；uchar 槽一切写法折 cmpb/test。
    // V2（FULL 内联 if + 单 stub）实测劣化：消掉 1 行 stub jmp 但反出
    // cond2 方向（je↔jne）+ 双 body 填参各 +2 行装载序翻转 ⇒ 净差 4 行
    // >1。维持 V1（双 goto 钉块序，残留 1 行 = 4.4 label-stub bb 的
    // fall-through jmp，N-JMP 族）。
    // [b28d-solo-W8LT_DBTask] 去 else 花括号（裸 if 链）：4.4 不再为 else 块出口
    // 发 fall-through jmp，SIMPLE_SQL 直接跟随条件跳转（ORIG 形）。
    // [J4-r1] V3 直写 `if(!flag || days==0) A else B`（P214①/P258 德摩根族）：
    // 目标是复现 ORIG 0x8421b5e `jne A` + 0x8421b64 `jne B` 后**直接落 A**
    // （无 label-stub jmp/nop）。
    if (!flag || days == 0)
    {
        db->set_query(
            "upDate charac_stat set forbidden_to_play=0, forbidden_due_to= 0 where charac_no = %d",
            characNo);
    }
    else
    {
        db->set_query(
            "upDate charac_stat set forbidden_to_play=1, forbidden_due_to = date_add(now(), interval %d day) where charac_no = %d",
            days, characNo);
    }
    if (!db->exec(true))
        return 0;
    return 1;
}