void PRINT_ERROR::find_item(char* tag, CUser* user, int itemId)
{
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x2ac,
        "%s : find_item() fail [item_id:%d] [charac_no:%d]",
        tag, itemId, (unsigned int)user->getCurCharacNo());
