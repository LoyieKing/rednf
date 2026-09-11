// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save)
{
    // [b28d-solo-AvatarRechargeServer] AE：ORIG（idx66 报告）Acquire 结果直传
    // CStreamGuard ctor 出参槽（P0 无具名局部）；make_internal_stream_jpn 后经
    // operator->() 再 put_binary。旧 DIFF 根因 = 实参冗余 `(char*)` cast 令 4.4
    // 提前把 &save 升格 %ebx 跨 call 保活；删 cast 写 `put_binary(&save, 8)` 后
    // 复现 ORIG `mov 0x8(%ebp),%edx; mov %edx,0x4(%esp)`（52=52，compare_tu AE）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f),
        true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x31, 0xffffffff);
    guard->put_binary(&save, 8);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}