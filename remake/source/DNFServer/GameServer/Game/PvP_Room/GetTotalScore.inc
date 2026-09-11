// [b28d-7 N-帧量化] ORIG 0x085dc034 `sub $0x4` vs 4.4 `sub $0x14`：callee PvpResultType::GetTotalScore 定义在 PvpResultType.cpp（跨泳道，仅声明可见）⇒ 4.4 保守 outgoing 量化（P190 附注②）；指令序列全等、P167 CLEAR。
// [b28d-solo-PvP_Room] N-P190: ORIG 0x085dc034 sub $0x4 vs 4.4 $0x14 outgoing 量化；callee PvpResultType::GetTotalScore 跨 TU 仅声明
// [L10-2 P210②] GetTotalScore 定义迁入本 TU 并置于 caller 之前 ⇒ 4.4 精确继承
// outgoing（同 PvpResultType.cpp 内 PvpResultSet 三薄封装的前例）。原地址
// 0x085d5efe 与 PvP_Room 段相邻，属 ORIG 同 TU。
int PvpResultType::GetTotalScore() const
{
    return m_avg[11] + m_avg[12] + m_avg[13] + m_avg[14] + m_field6c +
           m_field70 + m_field74 + m_field78;
}