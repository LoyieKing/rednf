// [b28d-62] N18a：历史判词保留（死 regmove 两段，三档类型旋钮已穷举）。
//   ORIG 0x08545276
bool CItemMakingSkill::_saveData(CUser* pUser)
{
    // b28c-L5 代落判词（lane14 件，DELEGATED→本 TU 属主）：87→86，残差为
    //   `mov disp(%eax),%eax; mov %eax,%edx` 两段死 regmove —— 目标 %edx 是
    //   caller-saved 且值不跨 call 保活，按 lane14/lane28 终版口径**不适用**裸
    //   register（裸 register 只在目标为 callee-saved 且跨 call 时可能非劣化）；
    //   三档类型旋钮（callee 返回类型 / 字段同异型 / store 方向）已由 lane14 穷举
    //   ⇒ 判 N18a 封顶。本文件内我波次真修的是 IsAbleItemUnlock/Cancel 两处
    //   「返回值被预清局部吞掉」的语义硬伤（见其上函数头判词）。
    // [N1D 2026-09-11 §6.5 串盲区修正] ORIG .rodata 实读为 "ItemMakingSkill.cpp"
    //   （ORIG 0x0854528b `movl $"ItemMakingSkill.cpp",0x4(%esp)`）；我方原写
    //   "CItemMakingSkill.cpp" 多 "C" 前缀 ⇒ StreamPool 记录源名与 ORIG 不一致。
    //   指令流零变化（AE 保持），仅串内容归位。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("ItemMakingSkill.cpp", 0x33),
                       true);
    guard.operator*()->operator<<((int)0x26e);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    // ORIG：GetInBuffer 结果落唯一槽 -0x10（sigBuf）→ memset 0x1c →
    // get_charac_no(-1) 返回值经 %edx 直存 *(int*)sigBuf（值先地址后，无 callee-saved
    // 缓存）。skills[11]@+4（ushort ×11）循环内槽直读重物化。
    // [b28d-63 本 TU] 具名局部用视图指针类型：ORIG 0x0854533a 在 call 之后才载基址
    //   （`mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,(%eax)`），char* 形 4.4 恒
    //   LHS-first 并把基址提前缓存进 %ebx。
    ItemMakingSkillReqView* sigBuf =
        reinterpret_cast<ItemMakingSkillReqView*>(
            guard.operator->()->GetInBuffer<SIG_ITEM_MAKING_SKILL>());
    memset(sigBuf, 0, 0x1c);
    // [M2S2 2026-09-11 · N76 判词作废] 旧判词「三形均不翻 ⇒ N76」已推翻：正解是
    //   **字段侧异型化**（P211 反形异型化）—— 视图 `ItemMakingSkillReqView::m_characNo`
    //   int → unsigned int（RHS `get_charac_no` 仍 int ⇒ 异型转换中转），就地翻回
    //   ORIG 0x08545335 的 RHS-first：`mov %eax,%edx; mov -0x10(%ebp),%eax;
    //   mov %edx,(%eax)`（我方旧 LHS-first `mov -0x10(%ebp),%edx; mov %eax,(%edx)`）。
    //   实测 86→87 条 = ORIG 87，**verdict DIFF→AE**（.o 口径 ae 48→49 / diff 6→5）。
    //   旧判词失败原因：当时未把字段改 unsigned（仅在 char* 局部形下试过「RHS cast」）。
    //   零 ABI 风险：视图结构仅本 TU 内使用，宽/偏移/pack 不变。
    sigBuf->m_characNo = pUser->get_charac_no(-1);
    for (int i = 0; i <= 0xa; ++i)
        sigBuf->m_skills[i] = m_skillLevels[i];
    // ORIG：MsgQueueMgr::put(DB_Q=2, guard) 提交 DB 队列（0x08545381）。
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}