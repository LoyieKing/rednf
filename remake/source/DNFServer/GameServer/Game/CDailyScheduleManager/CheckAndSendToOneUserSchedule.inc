// ORIG 0x8125cb8：CheckAndSendToUserSchedule(&user, false)。
void CDailyScheduleManager::CheckAndSendToOneUserSchedule(CUser& user)
{
    CheckAndSendToUserSchedule(&user, false);
}