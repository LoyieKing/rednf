// ORIG 0x0842a910 sz 0xeb（_ZN26DB_DeathTowerLoadTopRanker11makeRequestEb）：
// BigStreamPool::Acquire("DBThread.cpp",0x6c5f) → CStreamGuard(stream,true)
//   → <<0xe4(int) → <<-1(int) → <<param(bool) → put(DB_Q(2))。
// 报告：docs/class_func_reports/DB_DeathTowerLoadTopRanker/makeRequest.md。
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
void DB_DeathTowerLoadTopRanker::makeRequest(bool reload)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x6c5f), true);
    guard.operator*()->operator<<((int)0xe4);
    guard.operator*()->operator<<((int)-1);
    // ORIG +0x78 `cmpb $0,-0x1c; je; mov $1/0,%ebx`：分支即时直写 callee-saved
    // %ebx 跨 operator* 调用存活（register 局部实证 g2 同形；`v=c?1:0` 经
    // %eax 聚合多一条 mov %eax,%ebx；probe 验证）。
    // [b28d-solo] P167：ORIG 调 `CStreamGuard::lsEh`（unsigned char），
    // int 实参选中 lsEi（差集 -1/+1）⇒ 改 unsigned char。
    guard.operator*()->operator<<((unsigned char)(reload ? 1 : 0));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}