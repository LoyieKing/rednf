// ---- IsOpenScheduleDungeon() const —— ORIG 0x0834d2d4（报告全文） ----
// GlobalData::s_DailyScheduleManager.GetScheduleState(type=0, m_index) ∈ {1,2}。
bool CDungeon::IsOpenScheduleDungeon() const
{
    // ORIG @0x834d2da 函数头 `movl $0,-0x10` 死初始化槽（全函数无后续读写，
    // P171 照抄）。
    int deadSlot = 0;
    int state = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)0, (unsigned int)m_index);
    if (state == 1 || state == 2)
        return true;
    return false;
}