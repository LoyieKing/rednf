void CDailyScheduleManager::CheckAndSendToOneUserSchedule(CUser& user)
{
    CheckAndSendToUserSchedule(&user, false);
