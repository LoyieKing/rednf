char* CGenUniqueNo::genIPGNo(ENUM_IPGNO_TYPE type, unsigned int no, char* out)
{
    // b28d-solo-CGenUniqueNo（AE）：ORIG 0x084b7750。typeConst 是 ORIG 函数内局部静态
    // （_ZZN8WongWork12CGenUniqueNo8genIPGNoENS0_15ENUM_IPGNO_TYPEEjPcE9typeConst
    // @0x093abb44 d），故移入函数体，消除同名文件级静态发明符号。env/year 无具名槽
    // （env 内联、year 以 `lea 0x76c(%eax)` 直入 %ecx）；调用目标 OS_API::snprintf。
    // ORIG 该局部静态 nm size=8（无 NUL）；4.4 拒绝 `char[8]="CLQBVFOE"`，用逐字符列表同尺寸。
    static const char typeConst[8] = { 'C', 'L', 'Q', 'B', 'V', 'F', 'O', 'E' };
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm local_tm;
    struct tm* cur = localtime_r(&curSec, &local_tm);
    OS_API::snprintf(out, 0x14, "%c%02d%02d%02d%02d%010d",
                     (int)(signed char)typeConst[(unsigned int)type],
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (cur->tm_year + 1900) % 2000, cur->tm_mon + 1,
                     cur->tm_mday, no);
    return out;
