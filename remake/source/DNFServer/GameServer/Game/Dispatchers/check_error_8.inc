int Dispatcher_GetAchievementReward::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    if (!isOpenAdvanceAltar())     // [objdump 0x08140d80]
        return 1;
    return 0;
