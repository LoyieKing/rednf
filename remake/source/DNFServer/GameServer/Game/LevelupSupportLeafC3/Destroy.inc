// ============================================================================
// LevelupSupportEventManger::Destroy —— 报告 Destroy.md @0x08146fda。
// clearScript（释放堆上 vector）→ 清 m_stepIndexMap → 清 m_events。返回 void
// 见文件头说明。
// ============================================================================
int LevelupSupportEventManger::Destroy()
{
	m_script.clearScript();
	m_stepIndexMap.clear();

	m_events.clear();
	return 1;
}