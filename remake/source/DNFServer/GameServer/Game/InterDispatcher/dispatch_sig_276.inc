int Inter_LevelUpReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x62a6;
    unsigned int field0;
    unsigned int field4;
    memcpy(&field0, data, 4);
    memcpy(&field4, data + 4, 4);
    (void)field0; (void)field4;
    return 0;
