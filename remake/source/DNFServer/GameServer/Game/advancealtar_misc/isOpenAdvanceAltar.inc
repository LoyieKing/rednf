bool isOpenAdvanceAltar()
{
    const int state =
        GlobalData::s_DailyScheduleManager->GetScheduleState((_DailyScheduleType)4, 0xa);
    if (state == 1)
        return true;
    return false;
