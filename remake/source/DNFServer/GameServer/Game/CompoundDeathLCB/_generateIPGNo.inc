char* CCompoundAvatar::_generateIPGNo(const CUser* user, long curSec,
                                      char* buf) const
{
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333df6 直取形参地址（无 time_t t
    // 局部槽）且把 localtime_r 返回值存具名指针槽 -0x1c 后经 p-> 读字段
    //（P43/P0）；五个 snprintf 实参全内联、右起求值（characNo→mday→mon→
    // year%2000→serverId），characNo 升格 %esi 跨 G_CEnvironment 保活
    //（P226/P227 形）。
    // 【定案】达成 **AE**：56/56 逐条全等（仅字符串立即数重定位标签伪差）。
    struct tm lt;
    struct tm* p = localtime_r(&curSec, &lt);
    // ORIG 0x08333e63 `mov 0x1b0(%eax),%eax`（4 字节读）= CEnvironment+0x198
    //（m_serverEnvir）+0x18 → Server_Envir::m_channelNo（通道号；同
    // CDataManager.cpp:1461 具名口径）。
    OS_API::snprintf(buf, 0x14, "C%02d%02d%02d%02d%010d",
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     (p->tm_year + 1900) % 2000,
                     p->tm_mon + 1,
                     p->tm_mday,
                     user->getCurCharacNo());
    return buf;
