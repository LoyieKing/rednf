// ============================================================================
// 【报告缺失,objdump】PRINT_ERROR::find_item —— ORIG T 0x08197caf。
// "%s : find_item() fail [item_id:%d] [charac_no:%d]"
// file=localjapan/Arad_Function_Set.cpp line=0x2ac（归属建议：并入
// Arad_Function_Set.cpp 同族段；暂放本 TU 保证链接）。
// ============================================================================
void PRINT_ERROR::find_item(char* tag, CUser* user, int itemId)
{
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x2ac,
        "%s : find_item() fail [item_id:%d] [charac_no:%d]",
        tag, itemId, (unsigned int)user->getCurCharacNo());
}