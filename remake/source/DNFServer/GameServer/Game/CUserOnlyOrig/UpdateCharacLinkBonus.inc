// ---------------------------------------------------------------------------
// 27. CUser::UpdateCharacLinkBonus @0x0865240c
//     实参写流序:(bonusState@+0x18 先物化) 0x14c/uid/state/exp/gold/bonusState
//     —— asm:local_10=param5 缓存;写入序 sig:uid,state(+0x14 参3),exp(参1),
//     gold(参2),bonusState。按 Ghidra operator<< 顺序照录。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0865240c：p167=CLEAR，gap=3；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0865240c：N82 封顶——槽号平移/callee-saved
// （p167 CLEAR）。
void CUser::UpdateCharacLinkBonus(unsigned int exp, unsigned int gold,
                                  unsigned int state,
                                  ENUM_CHARAC_LINK_BONUS_STATE bonusState)
{
    // P0：ORIG 头 `mov 0x18(%ebp),%eax; mov %eax,-0xc(%ebp)`——bonusState 拷入
    // 具名局部槽，末次 operator<< 从槽重读（0x14/0x10 形参直读不受影响）
    unsigned int bs = static_cast<unsigned int>(bonusState);
    // [A06 2026-09-10 P259] ORIG 0x0865240c 无 stream 具名槽：`call Acquire` 结果
    // %eax 直落 guard ctor 实参（O11 `call`; O13 `mov %eax,0x4(%esp)`），-0xc 槽归 bs。
    // 旧具名 `Stream* stream` 多一对 store/load 并顶浅全部槽（gap 3）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1c11), true);
    // ORIG：guard.operator*()->operator<< 六处显式代理链；GetUID 传 int（选
    // operator<<(int)，否则误落 unsigned 重载——const 甄别）。
    guard.operator*()->operator<<((int)0x14c);
    guard.operator*()->operator<<((int)GetUID());
    guard.operator*()->operator<<(state);
    guard.operator*()->operator<<(exp);
    guard.operator*()->operator<<(gold);
    guard.operator*()->operator<<(bs);
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);

    setCharacLinkBonusExp(0);
    setCharacLinkBonusGold(0);
    // [G6Sub3 2026-09-10 复核判 N104+N132 封顶] 92/93、帧 0x20、槽位集(-0x14/-0xc)全等；
    // 残差两项：①ORIG 0x086524f5 正常路径尾（CStreamGuard::D1 @0x0865253d + epilogue）
    // **居函数最末**、unwind 块（0x08652549 `mov %edx,%ebx; mov %eax,%esi; … _Unwind_Resume`）
    // 居中；4.4 把 unwind 压中段、正常尾块后移并多发一条 `jmp`（+1 = 全部 gap）⇒ N104
    // 「EH 尾块相对序」。②数据地址差：`_ZN10GlobalData13s_stream_poolE`(0x940bd6c) /
    // `_ZN10GlobalData10s_msgq_mgrE`(0x940bd68) / "user.cpp" 字符串在 .o 显示为
    // `mov 0x0,%eax` / `$0x253`，属链接期段布局 ⇒ N132。语义零差。
}