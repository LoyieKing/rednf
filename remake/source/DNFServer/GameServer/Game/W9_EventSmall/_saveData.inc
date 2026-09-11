// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b，gap=-1/blocks=1）→ 试=①对照探针（4.4.6/4.4.7
//   同 -O0/-m32）：`局部指针->零偏移字段 = call 结果` 恒产 3 条 RHS-first
//   （`mov %eax,%edx; mov slot,%eax; mov %edx,(%eax)`，与本件我方完全一致），
//   仅**非零位移** store 才产 ORIG 的 2 条 LHS-first 形 ⇒ ORIG（4.1）对零位移产 2 条属
//   版本 lowering 差异；②prett/q3_gap2 缺口 0、strall 串 CLEAR（"CharacBloodDungeon.cpp"
//   与 ORIG 0x8c26d80 逐字节同）；③槽位与 ORIG 全等（sig -0xc/guard -0x14）⇒ 非 P339/P331
//   ⇒ 维持 N18b（m_characNo 位于 sig+0x00，无法借非零位移翻形）。
bool CCharacBloodDungeon::_saveData(CUser* pUser)  // ORIG 0x832a330
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CharacBloodDungeon.cpp", 0x1e),
                       true);
    guard.operator*()->operator<<(0x197);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_BLOOD_DUNGEON* sig = guard.operator->()->GetInBuffer<SIG_BLOOD_DUNGEON>();
    memset(sig, 0, sizeof(SIG_BLOOD_DUNGEON));   // ORIG memset 0x28
    // [P211 档③ 试形记录（已回滚）] 删 `(int)` cast 试同型化（ORIG 0x0832a3e7 为
    //   LHS-first 直存）实测 DIFF 数不变（3 件）⇒ 判 N18b/装载序族，保持直证形。
    // [M1S3EmblemLink 2026-09-11 复核封顶] 唯一残余 = 1 条：ORIG 0x0832a3e7
    //   `mov -0xc(%ebp),%edx; mov %eax,(%edx)`（RHS 留 %eax，指针装 %edx 后直存）
    //   vs 4.4 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`（多一条
    //   值搬运）⇒ 我方 113 vs ORIG 112。§6.5 串已核（.o 内
    //   "CharacBloodDungeon.cpp" 与 ORIG 0x8c26d80 字节一致）、§6.5.1 不适用
    //   （无 EUC-KR 字面量）；槽位与 ORIG 全等（sig -0xc、guard -0x14）⇒ 非 P339/
    //   P331 域，属 4.4 赋值 lowering（N18b 族）⇒ 封顶。
    // [P6S6 2026-09-11 复核·仍 N] 本波再删 `(int)` cast 实测：`.o` 反汇编
    //   仍是 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`（113 vs 112，
    //   blocks=1）⇒ 4.4 恒「地址入 %eax、值入 %edx」，ORIG 4.1 形
    //   `mov -0xc(%ebp),%edx; mov %eax,(%edx)`（值留 %eax 直存）源码不可择 ⇒ 维持 N。
    sig->m_characNo = (int)pUser->getCurCharacNo();
    sig->m_point = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount = m_weeklyEnterCount;
    sig->m_useGold = m_useGold;
    sig->m_dailyInitTime = getDailyInitializeTime(6, 0, 0);  // 覆写 +0x10
    // ORIG 跳过 this+0x14：sig+0x14←this+0x18、sig+0x18←this+0x1c（与 loadData
    // 镜像一致；原写 this+0x14/0x18 系真错位 bug）。
    sig->m_field14 = m_enterCount;
    sig->m_enterCount = m_field1c;
    sig->m_flag1c = m_flag20;                    // byte 镜像
    sig->m_field20 = m_field24;
    sig->m_field24 = m_field28;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_flag20 = 0;                                // ORIG 发送后清标志
    return true;                                 // 返回 1
}