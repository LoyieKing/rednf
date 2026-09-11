// ---------------------------------------------------------------------------
// TOD_UserState::ClearLayer() —— ORIG T 0x0864390e：层推进 + 脏标记 + 清层时间。
// ---------------------------------------------------------------------------
void TOD_UserState::ClearLayer()
{
    reinterpret_cast<charac_expand::CData*>(this)->alter();
    // [b23S3C] S23A.Sec TOD_UserState→内嵌 m_data（SIG_REQUEST_TOD_USER_STATE）
    // 迁移遗漏点修复（本 TU 编译红→绿；字段偏移经头 316-319 注释实证）。
    reinterpret_cast<TOD_Layer*>(&m_data.m_todayLayer)->increaseLayer();
    m_data.m_lastClearTime = GlobalData::s_systemTime_.getCurSec();
}