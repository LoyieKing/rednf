// ============================================================================
// dispatch @ 0x08437df4
// 基类 DBDispatcher::dispatch 检查失败返 0 → *stream>>characNo →
// Acquire 组包 → guard<<userIdx<<slot → GetInBuffer<SIG_LOAD_NO_CACHE_DATA>
// （NULL 返 0）→ memset 0x2c → sig+0=characNo → LoadBloodDungeonReward →
// MsgQueueMgr::put(DISPATCH_Q=1, guard) 返 1。
// ============================================================================
// [Lane31W8LTDBTaskView] 视图 cast 反模式清理：同型/同偏移视角已并入权威头
// （SigTypes_C.h SIG_LOAD_NO_CACHE_DATA 的 m_characNo/m_characNoInt 双视角），
// 本地 DB_LoadNoCacheDataIntView 删除。
// [b28d-4] DIFF ndiff2：P248 档③ 同型化后 gap=0（int 视图消 `mov %eax,%edx` 中转）；
// 残差 = ORIG RHS-first（`mov -0x10,%edx` 先于载目标）vs 4.4 同型 LHS-first，
// 异型形反多中转 ⇒ N18a/N76 封顶。ORIG 08437df4。
// [b28d-solo-2R] NEAR(2) 复核：视图字段 int→unsigned 仍 LHS-first（2 条 mov 序差）；
// 直接 `sig->m_characNo = characNo`（权威 unsigned 字段 + int RHS）反多 `mov %eax,%edx`
// 中转 → DIFF。RHS-first 无中转三形不可造 ⇒ N76/N18a 封顶。ORIG 08437df4。
// [b28d-solo-4] P211 边界复核（探针 /tmp/probe_store{2,3}.cpp）：LHS 指针为**局部**
// 时 4.4 恒 LHS-first，字段/RHS 类型旋钮（int/unsigned/pack1/视图/裸解引用）全无效；
// 仅**形参**指针给 RHS-first ⇒ 本条属 N76/N18a 硬封顶（P211 档③不适用）。
bool DB_LoadNoCacheData::dispatch(int userIdx, int slot, Stream* stream)
{
    int characNo;
    SIG_LOAD_NO_CACHE_DATA* sig;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    characNo = 0;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8549), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_LOAD_NO_CACHE_DATA>();
    if (sig == 0)
        return 0;
    memset(sig, 0, 0x2c);
    // P248 档③（b28d-4）：SIG 字段为 unsigned int 而 RHS 是 int 局部
    // （ORIG `Stream::rsERi` 实证）⇒ 4.4 恒插 `mov %eax,%edx` 转换中转；
    // ORIG 0x08437e.. store 为单段 RHS-first（无中转）。用本 TU int 视图
    // 与 RHS 同型化（布局同宽、sizeof/偏移不变）。
    sig->m_characNoInt = characNo;
    LoadBloodDungeonReward(sig);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}