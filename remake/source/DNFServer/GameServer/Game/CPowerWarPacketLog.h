// df_game_r 还原 —— CPowerWarPacketLog（PowerWar 包日志聚合器）。
// 布局依据 ORIG C1 0x0848085a / D1 0x0848089c：仅构造 std::map<int,STPacketLog*>
// 成员（this+0，sizeof 0x18）并 Reset；无 vtable（C1/D1 无 vptr 写入，nm 无
// _ZTV18CPowerWarPacketLog），析构为非虚。CPowerManager 于 +0x1f0 嵌入本对象
// （ORIG CPowerManager::CPowerManager 0x0847ea31 调本类 C1 / 0x0847ea8d 调 Reset）。
#ifndef DNF_GAME_CPOWERWARPACKETLOG_H_
#define DNF_GAME_CPOWERWARPACKETLOG_H_

#include <map>

#include "STPacketLog.h" // enum eSendType / struct STPacketLog

class CPowerWarPacketLog
{
public:
    CPowerWarPacketLog();                          // ORIG T 0x0848085a
    ~CPowerWarPacketLog();                         // ORIG T 0x0848089c（非虚）
    void InsertPacketLog(int i, unsigned short t,
                         eSendType type, char const* name);  // ORIG T 0x08480970
    void PrintPowerWarPacketLogReport();           // ORIG T 0x08480ac0
    void Reset();                                  // ORIG T 0x084808de

    std::map<int, STPacketLog*> m_map;             // +0x00（sizeof 0x18）
};   // sizeof == 0x18


#endif // DNF_GAME_CPOWERWARPACKETLOG_H_
