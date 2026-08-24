#ifndef DNF_GAME_APSYSTEM_TYPES_H_
#define DNF_GAME_APSYSTEM_TYPES_H_

// CUser 完整定义见 CUser.h（本头仅签名引用）
class CUser;

// APSystem 共享类型（CActionPointEtcParameter 成员元素 / DB 签名）。
// ORIG 证据：ClearData 0x088948fc、IsValidTodayRewardItem 0x08894976
// （_SIG_LOAD_ACTION_POINT16_TodayRewardItemE 嵌套类，单 int 按 4 字节传递）。
// 由 MoreTypes.h 与 CDataManager.h 共同包含，避免重复定义。

class MSG_CHARAC_SELECT;   // 全局作用域（PacketTypes.h 权威定义；仅引用）
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


// ORIG 0x08121616：GetActionPointFromMemory（纯空函数）
// ORIG T 0x08121608：RequestLoadActionPointFromDBAndES（ORIG 体仅 return 1，
// 13 字节；SelectCharac::process 调用）。
class CDBAndExchangeServerProc
{
public:
    static void GetActionPointFromMemory(const CUser& user, _SIG_LOAD_ACTION_POINT& out);
    static bool RequestLoadActionPointFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg);
};
}

#endif  // DNF_GAME_APSYSTEM_TYPES_H_
