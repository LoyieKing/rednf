// df_game_r Game/ ARAD::Arad_EventPeriodDataManager（批3：从 GameStubs.cpp 迁移）。
//
// ORIG（df_game_r, elf32-i386）：
//   class ARAD::Arad_EventPeriodDataManager（namespace ARAD）
//   类名 "Arad_EventPeriodDataManager" = 27 字符
//   bool isApplied(ENUM_REPEAT_EVENT_CODE, unsigned int)
//   mangled: _ZN4ARAD27Arad_EventPeriodDataManager9isAppliedE22ENUM_REPEAT_EVENT_CODEj  @ 0x081956b0
//
// 注意：ENUM_REPEAT_EVENT_CODE 必须位于全局命名空间（非 ARAD 内），
//   否则 Itanium ABI 会生成 NS_22ENUM_REPEAT_EVENT_CODEE 而无法匹配 ORIG 的
//   E22ENUM_REPEAT_EVENT_CODE 符号（已用 g++ 实测验证）。

#ifndef DNF_ARAD_EVENTPERIODDATAMANAGER_H
#define DNF_ARAD_EVENTPERIODDATAMANAGER_H

#include <vector>

// 全局作用域枚举（ORIG ABI 要求，勿移入 ARAD 命名空间）。
enum ENUM_REPEAT_EVENT_CODE
{
    ENUM_REPEAT_EVENT_CODE_NONE = 0,
    ENUM_REPEAT_EVENT_CODE_0x8e = 0x8e,   // ResetAllFatigue 中用于判断的重复事件码
};

namespace ARAD
{
namespace SCRIPT
{
// ARAD::SCRIPT::SEventPeriodData —— isApplied 遍历的事件周期数据项。
//   +0x00 m_eventCode  事件码（SearchEventPeriod 匹配键）
//   +0x04 m_beginTime  生效开始时间（含）
//   +0x08 m_endTime    生效结束时间（不含）
struct SEventPeriodData
{
    unsigned int m_eventCode;
    unsigned int m_beginTime;
    unsigned int m_endTime;
};
}  // namespace SCRIPT

class Arad_EventPeriodDataManager
{
public:
    // ORIG 0x081956b0：事件 code 在有效周期内（m_beginTime <= time < m_endTime）返回 true。
    bool isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time);

    // ORIG 0x08195686：以当前 tick 为 time 调用 isApplied（供调用方/宿主迁移参考）。
    bool isRunning(ENUM_REPEAT_EVENT_CODE eventCode);

private:
    // ORIG 类从 Arad_InterfaceGameScript 派生（vtable + 基类共 0xc 字节），
    // 成员的 +0x0c 起为 vector<SEventPeriodData>。此处用 0xc 字节 pad 保持该成员偏移，
    // 使该方法在宿主经 findGameScript 获得的真实 ORIG 布局对象上也能正确工作。
    char m_pad[0xc];
    std::vector<ARAD::SCRIPT::SEventPeriodData> m_eventPeriodList;  // +0x0c
};

}  // namespace ARAD

#endif  // DNF_ARAD_EVENTPERIODDATAMANAGER_H