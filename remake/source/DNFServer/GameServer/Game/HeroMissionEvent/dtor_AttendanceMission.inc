AttendanceMission::~AttendanceMission() {}
// ============================================================================
// AttendanceMission::processMission —— ORIG T 0x08166002（processMission.md）。
// 签到：以 fatigueTime 为每日重置时刻；m_extra[0] 为签到上限、[1] 为开始
// 月份、[2] 为开始日；与目标日相差超过 1 天（0x15180 秒）先 reset，再
// field4+1（钳制到上限）并 field8 累加相差天数。
// ============================================================================
