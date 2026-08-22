#ifndef DNF_GAME_APSYSTEM_TYPES_H_
#define DNF_GAME_APSYSTEM_TYPES_H_

// APSystem 共享类型（CActionPointEtcParameter 成员元素 / DB 签名）。
// ORIG 证据：ClearData 0x088948fc、IsValidTodayRewardItem 0x08894976
// （_SIG_LOAD_ACTION_POINT16_TodayRewardItemE 嵌套类，单 int 按 4 字节传递）。
// 由 MoreTypes.h 与 CDataManager.h 共同包含，避免重复定义。

namespace APSystem
{
class _SIG_LOAD_ACTION_POINT
{
public:
    class _TodayRewardItem
    {
    public:
        int m_itemIdx;  // +0x00
    };
    char m_pad[0x100];
};

// 元素类型占位（布局后续批次落地；当前 CActionPointEtcParameter 仅 clear）
struct CActionPointEx {};      // vector<CActionPointEx> 元素
struct _MedalRewardItem {};    // vector<_MedalRewardItem> 元素
}

#endif  // DNF_GAME_APSYSTEM_TYPES_H_
