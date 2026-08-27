// ============================================================================
// WongWork::CEventMgr —— ORIG df_game_r 0x0848ef0c..0x0848f1a2（静态事件查询）
// 参照 docs/class_func_reports/WongWork__CEventMgr/（7 函数报告）。
// 注意：与 game::CEventManager（运行时事件管理器，GlobalData::s_event_manager）
// 完全不同的类；本类是无状态静态查询（硬编码 2006 年活动窗口）。
// ============================================================================
#ifndef __CEVENTMGR_H__
#define __CEVENTMGR_H__

struct tm;

namespace WongWork
{

class CEventMgr
{
public:
    // ORIG 枚举名仅出现在 mangled（NS0_10EVENT_TYPEE）；字面值语义：
    // 0=普通日期窗口判断，1=额外要求当天为周末（周六/周日）【推断命名】。
    enum EVENT_TYPE
    {
        EVENT_PLAIN = 0,
        EVENT_WEEKEND = 1
    };

    // ORIG 0x0848ef0c _ZN8WongWork9CEventMgr11_IsEventingEiiiiiiiiNS0_10EVENT_TYPEE
    static bool _IsEventing(int startYear, int startMon, int startDay, int startHour,
                            int endYear, int endMon, int endDay, int endHour,
                            EVENT_TYPE type);

    // ORIG 0x0848f078 _ZN8WongWork9CEventMgr22GetPremiumServiceBonusEv
    static int GetPremiumServiceBonus();
    // ORIG 0x0848f140 _ZN8WongWork9CEventMgr22IsChildrensDayEventingEv
    static bool IsChildrensDayEventing();   // W6_CeraShopDeps 消费
    // ORIG 0x0848eff8 _ZN8WongWork9CEventMgr13GetCoinPerDayEv
    static int GetCoinPerDay();             // ORIG 恒返 1
    // ORIG 0x0848f06e _ZN8WongWork9CEventMgr20GetMoneyCharacCreateEv
    static int GetMoneyCharacCreate();      // ORIG 恒返 0
    // ORIG 0x0848f0dc _ZN8WongWork9CEventMgr16IsPCRoomEventingEv
    static bool IsPCRoomEventing();         // ORIG 2006-01-24 00 ~ 2006-02-07 06

    // ORIG T 0x0848f002：新角色初始送币（GetRepeatEvent(5)->IsEventing 命中时
    // 取 CCoinEventOnCharCreate::GetCoinNoOnCreateCharac，默认 5）。
    // 调用方 arad_sp_create_inven；定义在 ARAD_Database.cpp（W6_ARAD 批次）。
    static int GetCoinCharacCreate();
};

} // namespace WongWork

// ORIG 0x086b1d86 _Z7in_timeP2tmS0_ —— 全局自由函数：now ∈ [mktime(begin), mktime(end))
bool in_time(struct tm* begin, struct tm* end);

#endif // __CEVENTMGR_H__
