// ---- PRINT_ERROR::CheckItemLock @ 0x08197bfd -------------------------------
// N96 清绑：同 isEmpty（21=21，与 ORIG 全等）。
void PRINT_ERROR::CheckItemLock(char* tag, CUser* user, unsigned short slot)
{
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x2a2,
        "%s : CheckItemLock() fail [slot:%d] [charac_no:%d]",
        tag, (unsigned int)slot, (unsigned int)user->getCurCharacNo());
}