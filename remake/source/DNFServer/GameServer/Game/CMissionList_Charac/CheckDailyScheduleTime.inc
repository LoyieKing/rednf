bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；与 CInventory.h
// 的占位版本不同，本 TU 使用真实形态）


// [b28d-solo-CMissionList_Charac] P154 发明符号清理：原 `static int
// GetDailyScheduleTime(CEnvironment*)` 在 ORIG nm 中**不存在**（仅 T
// CheckDailyScheduleTime），ORIG loadData 0x085e49d0 直接内联
// `mov 0x37c(%eax),%eax`（= m_serverEnvir.m_fatigueTime，CEnvironment.h 权威布局）
// ⇒ 删除该 helper，调用点改为具名字段内联。

// ============================================================================
// 构造 / 析构 / 虚接口
// ============================================================================
