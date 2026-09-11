// [b28d-62] 修：CStreamGuard operator*/-> 链 + 非 const list iterator + 结构赋值
//   （P103/P132/P96/N84）。ORIG 0x085f7998
// [b28d-63 判词] 已修：stream 具名局部删（P259 内联实参）、日志串改 ORIG 原文
//   "RedeemItem.cpp"（原 "Redeem_Item.cpp"）、头部字段改 RedeemListHeadView 视图。
// [M2S2 2026-09-11 三项修正] ①旧「槽号整体 +4（N95/N126）」作废：根因是多余的第二个
//   具名局部 `char* sigBuf`（两个指针槽 -0x24/-0x20，ORIG 只一个）——删 sigBuf、
//   局部直接用视图指针类型后，**全部 5 槽与 ORIG 逐一命中**（guard -0x2c / begin -0x30 /
//   end -0x24 / sig -0x20 / i -0x1c），134→133 条。②characNo store 用 P211 字段异型化
//   （RedeemListHeadView::m_characNo int→unsigned）翻回 ORIG 的 RHS-first + %edx 中转，
//   与 count store 一并逐条同形。③唯一残差 = 循环体求值序（ORIG 先 `call operator*`
//   取源、再算目的地址并把 i 升格 %ebx；4.4 恒先算目的地址入 %ebx）—— P226 裸语句
//   表达式包 RHS 实测零影响（133 条不变，已回滚）⇒ N76/N18a 族后端求值序，gap -2。
bool CRedeemItem::_saveData(CUser* pUser)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // [b28d-63 本 TU] P259：Acquire 结果内联进 CStreamGuard 实参（ORIG 无 stream 槽）；
    //   文件名按 ORIG 0x085f79ae 为 "RedeemItem.cpp"（原 "Redeem_Item.cpp" 为日志串错）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("RedeemItem.cpp", 0x108), true);
    // [b28d-62] P103/P132 + P96：ORIG 走 operator* 2 次 + operator-> 1 次
    //   （p167 MISSING 实证）；list 迭代器用**非 const** iterator（const_iterator
    //   多 2 次转换构造 + 整套 const 版 operator*/*++/!= = EXTRA 实证）。
    **guard << (int)0x1f4;
    **guard << (int)pUser->GetUID();

    // [M2S2 2026-09-11] P0/P183/P329：原源码有两个具名局部（`sig` + `char* sigBuf`）
    //   占 -0x24/-0x20 两槽，ORIG 只有一个指针槽（-0x20）⇒ 每处使用多一轮
    //   `mov -0x24(%ebp),%eax; mov %eax,-0x20(%ebp)` 槽往返。现把 cast 内联到
    //   使用点、局部本身即视图指针类型（与 CItemMakingSkill::_saveData 同手法），
    //   复现 ORIG 单槽 + RHS-first store 形。
    RedeemListHeadView* sig = reinterpret_cast<RedeemListHeadView*>(
        guard->GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>());
    if (sig == 0)
        return false;
    memset(sig, 0, 0x59c);
    // SIG_REDEEMINFO_LIST_TO_DB 布局：characNo@+0 / count@+4 / entries[21]@8。
    // [b28d-63 本 TU] 视图字段（disp store）：ORIG 0x085f7a78 为
    //   `call size; mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)`（无 lea）。
    sig->m_characNo = pUser->get_charac_no(-1);
    sig->m_count = (int)m_redeemList.size();
    int i = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::iterator it =
             m_redeemList.begin();
         it != m_redeemList.end(); ++it)
    {
        // [b28d-62] N84：ORIG 用结构整体赋值（rep movsl），显式 memcpy 生 call。
        // [M2S2 2026-09-11 试形·负结论] P226 裸语句表达式包 RHS（`= ({ *it; })`）
        //   实测对循环体求值序**零影响**（仍先算目的地址入 %ebx 后 call operator*，
        //   133 条不变）⇒ 回滚。ORIG 0x085f7aa4 的「call 先 / dst 后 + `mov %edx,%ebx`
        //   两条 RHS 保活」属 4.1↔4.4 求值序（N76/N18a 族），源码不可控。
        // [N1D 2026-09-11 试形] ++i 从 for-increment 移入 body：ORIG 0x085f7aac
        //   顺序为 `rep movsl → addl $0x1,-0x1c(%ebp) → call operator++(it)`；
        //   4.4 原先把 ++i 排在 operator++ 之后。
        // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N76/N18a，件表文件名 _saveData_1.inc
        //   系映射偏差：本函数体在 _saveData.inc）→ 试=①prett2 0 缺口、strall 0 串差、
        //   p167 CLEAR；②本 TU 已路由 446 后端（实测读数不变）；③444 后端变体实测
        //   逐项相同 ⇒ 无后端旋钮；④ORIG 0x085f7aa4 先 `call operator*`（源→%edx）
        //   后算 dst，4.4 恒先算 dst 入 %ebx（P226 裸语句表达式历史已试无效）⇒ 维持
        //   （新=DIFF 135/134 gap=+1, blocks=5：唯一残差为该求值序 + `mov %edx,%ebx`
        //   RHS 保活两条 + GI 假阳性计数）。
        *(Inven_Item_Expand_RedeemInfo*)(
            reinterpret_cast<char*>(sig) + 8 + i * 0x44) = *it;
        ++i;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}