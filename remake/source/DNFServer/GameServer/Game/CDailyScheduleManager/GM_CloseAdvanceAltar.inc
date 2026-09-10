void CDailyScheduleManager::GM_CloseAdvanceAltar() { GM_CloseSchedule(10, (_DailyScheduleType)4, m_advanceAltar); }

// ---- SouthernDale（GM_OpenSouthernDale 0x81267bc / GM_CloseSouthernDale 0x8126ad2）----
// 打开：第一目标 (m_idx==100 && m_type==0) 回填 this+0x0（m_southernDaleA），
// 第二目标 (m_idx==0x5045 && m_type==1) 回填 m_southernDale(+0x24)；
// 各自把条目 m_wday 置 7，起始 h/m/s = 当前时刻，结束 h/m/s = 当前+600s；
// 关闭：把两目标各自写回 m_schedules 中匹配条目。
