void IBuyRule::LogValueStatistic(CUser* user, unsigned int value)
{
    // ORIG 0x85fb886：
    // GetInstanceValueStatistic()->AddValueStatistic(
    //     (VALUE_STATISTIC_FIELD)0xd, user, value)
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd,
                                                   user, value);
