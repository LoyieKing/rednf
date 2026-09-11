// ---- PRINT_ERROR::isEmpty @ 0x08197ba4 ------------------------------------
// N96 清绑：characNo 无槽 ⇒ 实参内联右值（21=21，与 ORIG 全等）。
void PRINT_ERROR::isEmpty(char* tag, CUser* user, unsigned short slot)
{
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x29d,
        "%s : isEmpty() [slot:%d] [charac_no:%d]",
        tag, (unsigned int)slot, (unsigned int)user->getCurCharacNo());
}