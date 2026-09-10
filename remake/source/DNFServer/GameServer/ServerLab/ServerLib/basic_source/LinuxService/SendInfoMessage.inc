void nsl::LinuxService::SendInfoMessage(ENUM_INFOTYPE infoType)
{
    char buf[10] = {0};
    buf[0] = (char)infoType;
