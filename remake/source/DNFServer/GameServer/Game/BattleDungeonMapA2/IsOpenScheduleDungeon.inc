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
