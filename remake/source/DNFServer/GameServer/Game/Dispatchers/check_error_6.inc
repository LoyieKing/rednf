int Dispatcher_UpgradeGage::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x08140314] mov $0x0,%eax; ret
