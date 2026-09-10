int LevelupSupportEventManger::Destroy()
{
	m_script.clearScript();
	m_stepIndexMap.clear();

	m_events.clear();
	return 1;
