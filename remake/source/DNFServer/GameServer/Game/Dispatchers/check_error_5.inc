int Dispatcher_SummonUnit::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x081405ae] mov $0x0,%eax; ret
