// ORIG @0x0846d6c2：time(&t) → localtime_r(&t,&tm) → sprintf 时间串。
void CExceptLog::Set_Time_String(char* buf)
