int Dispatcher_GetAchievementReward::process_packet(CUser* user,
                                                    CMDPacketStruct::STBaseRequest& req,
                                                    CMDPacketStruct::STBaseResponse& resp) const
{
    (void)user; (void)req; (void)resp;
    // [objdump 0x08140da0] mov $0x0,%eax; ret（ORIG 空体：不读请求、不写响应）
    return 0;
}