#include "CGenUniqueNo.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CEnvironment.h"

#include <stdio.h>
#include <time.h>

namespace WongWork
{
unsigned short CGenUniqueNo::m_nTimeCount = 0;

CGenUniqueNo::CGenUniqueNo()
{
}

void CGenUniqueNo::_updateTimeCount()
{
    ++m_nTimeCount;
    if (m_nTimeCount > 49999)
        m_nTimeCount = 1;
}

char* CGenUniqueNo::GenUniqueNo(char* out)
{
    _updateTimeCount();
    // b28d-36（修法）：补具名 channelNo 局部后帧/槽/寄存器全对齐（ORIG
    // `push %ebx; sub $0x34`、sec@-0x10、channelNo@-0xc、tc→%ebx），DIFF→NEAR。
    // 残余 N 封顶（N88/P75）：ORIG 在第一个 G_CEnvironment 后即取 channelNo
    // (0x378) 存槽、tc 之后才取 serverGroup(0x1b0)；4.4 把无副作用具名初始化
    // 惰性推迟到 snprintf 实参求值处 ⇒ 0x378/0x1b0 两次取用顺序互换（各 1 条
    // mov，指令总数/槽位集/寄存器集全等）。P48 双内联调用、参数序不变。
    int sec = GlobalData::s_systemTime_.getCurSec();
    // b28d-36 试形：ORIG 0x084b76ce 把 channelNo 提前物化到槽 -0xc（具名局部），
    // 4.4 原形把它直存 %ebx。补具名局部对齐槽往返。
    int channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
    snprintf(out, 0x14, "%02d%02d%10d%05d",
             channelNo,
             G_CEnvironment()->m_serverEnvir.m_serverGroup,
             sec, (unsigned int)m_nTimeCount);
    return out;
}

static const char typeConst[] = "CLQBVFOE";

char* CGenUniqueNo::genIPGNo(ENUM_IPGNO_TYPE type, unsigned int no, char* out)
{
    // b28d-36（修法，P0/P43）：ORIG 0x084b7750 无 env/year 具名槽——env 直接
    // 内联 `G_CEnvironment()->...`（调用点落在除法之后），year 以 `lea 0x76c(%eax)`
    // 直入 %ecx。删两个具名局部后帧 -0x54→-0x4c、cur -0x24→-0x1c 全对齐。
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm local_tm;
    struct tm* cur = localtime_r(&curSec, &local_tm);
    snprintf(out, 0x14, "%c%02d%02d%02d%02d%010d",
             (int)(signed char)typeConst[(unsigned int)type],
             G_CEnvironment()->m_serverEnvir.m_channelNo,
             (cur->tm_year + 1900) % 2000, cur->tm_mon + 1, cur->tm_mday, no);
    return out;
}
}
