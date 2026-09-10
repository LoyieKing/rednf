void CUser::IncrementTradeGold(unsigned int gold, int mode)
{
    if (mode != 0) {
        // [b28d-22 修法 P198①/P259/P222] 三处对齐 ORIG：
        // ① over 声明移入 if 块（ORIG 0x0864659x 预清在 mode!=0 之后，槽 -0x9 非头部 -0xd）；
        // ② Acquire 结果免具名 s 直落 guard ctor（ORIG 0x0864663x `call Acquire; movl $1,0x8(%esp);
        //    mov %eax,0x4(%esp); lea -0x14(%ebp)`，无 -0xc 中转槽）；
        // ③ 尾守卫 ORIG `sete %al; test; jne <skip>`（==0 物化）⇒ 负极性空 then 形。
        bool over = false;
        // ORIG 0x0864659x：`cmp $0x98967f,%eax; setbe %al; test %al,%al; je <skip>;
        // movb $0x1,-0x9(%ebp)` = 预清 false + `if (cond) over = true;` 两句式
        // （P198①；直接 `over = cond` 4.4 折成单条 mov %al,-0x9 少 test/je/movb）。
        if (GetOverTradeGoldCount() < 10000000u)
            over = true;
        SetOverTradeGoldCount(GetOverTradeGoldCount() + gold);
        if (over) {
            if (GetOverTradeGoldCount() >= 10000000u) {
                // 越界一次性上报（ORIG "user.cpp":0x408 处 Acquire）
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x408), true);
                // ORIG：guard.operator*()->operator<<(…) 显式代理链（operator* 调用点
                // 补齐；SetCharacInfo/SaveInventory 判例）。
                guard.operator*()->operator<<((int)0xd0);
                guard.operator*()->operator<<((int)GetUID());
                guard.operator*()->operator<<((unsigned int)get_acc_id());
                GlobalData::s_msgq_mgr->put(
                    (MsgQueueMgr::QUEUE_IDX)2, guard);
            }
        }
    }
    // ORIG 0x0864xxx 尾：`sete %al; test; jne <epilogue>` + body 尾 `jmp <epilogue>`
    // ⇒ ==0 早返形（空 then + jmp 岛）。
    if (getCurCharacR() == 0)
        return;
    IncrementCurCharacTradeGoldDaily(gold);
