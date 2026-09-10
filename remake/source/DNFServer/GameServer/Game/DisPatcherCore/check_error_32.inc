int DisPatcher_CheckSecurityProtection::check_error(CUser* /*user*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08264912：恒定返回 0
