#ifndef DNF_GAME_WONGWORKEVENTS_A_H_
#define DNF_GAME_WONGWORKEVENTS_A_H_

// ============================================================================
// WongWork 事件域（Alttle / Sigong / Everland / FCCombo / EventCommon）+ 杂项
// POD 的权威声明（本批 ORIG 符号全部经 nm -C df_game_r 核实）。
//
// 注意：ORIG 中 HackTypeInfo_t 为 W（inline 方法），故方法实现放头内以产生
// 弱符号；GlobalData.cpp 内现有的 struct HackTypeInfo_t{char m_pad[0x10];}
// 本地镜像仅服务于该 TU 容器实例化（sizeof 一致 0x10，ABI 兼容）；后续若该
// TU 收编本头，应删除本地镜像统一归口。
// ============================================================================

#include <string>
#include <vector>

#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE（NS_13ENUM_HACKTYPEE 锁定）
#include "MySQL.h"           // MySQL（GetDBHandle 返回类型）

namespace WongWork
{

// ----------------------------------------------------------------------------
// 独立 stHackInfo_t（非 CHackAnalyzer 嵌套视图）。
// ORIG: _ZN8WongWork12stHackInfo_tD1Ev/_D2Ev = W @0x080fb5c8, size 5（空析构）。
// 字段布局沿用 reportHackInfo/_addHackInfo 使用视图（uid/characNo/hackType/value）。
// ----------------------------------------------------------------------------
struct stHackInfo_t
{
    unsigned int   m_uid;        // +0x00
    unsigned int   m_characNo;   // +0x04【推断命名】
    unsigned short m_hackType;   // +0x08
    unsigned short m_pad0a;
    int            m_value;      // +0x0c

    ~stHackInfo_t();             // inline 定义见下，产生 W D1/D2 弱符号
};

inline stHackInfo_t::~stHackInfo_t() {}

// ----------------------------------------------------------------------------
// HackTypeInfo_t：6×u16 + u32@0xc，sizeof = 0x10。
// ORIG: C1Ev    W 0x086952b8 (0x44) —— 逐字段清零；
//       C1(...)  W 0x084eb7de (0x75) —— 按参数序写 +0/+2/+4/+6/+8/+a/+c；
//       CopyStruct W 0x086952fc (0x63) —— 逐字段拷贝。
// ----------------------------------------------------------------------------
class HackTypeInfo_t
{
public:
    HackTypeInfo_t();
    HackTypeInfo_t(ENUM_HACKTYPE type1, unsigned short value1,
                   ENUM_HACKTYPE type2, unsigned short value2,
                   unsigned short value3, unsigned short value4,
                   unsigned int limitCnt);
    void CopyStruct(const HackTypeInfo_t& other);

    ENUM_HACKTYPE  m_type1;     // +0x00（按 u16 传递/存储）【推断命名】
    unsigned short m_value1;    // +0x02
    ENUM_HACKTYPE  m_type2;     // +0x04
    unsigned short m_value2;    // +0x06
    unsigned short m_value3;    // +0x08
    unsigned short m_value4;    // +0x0a
    unsigned int   m_limitCnt;  // +0x0c
};

inline HackTypeInfo_t::HackTypeInfo_t()
{
    m_type1 = (ENUM_HACKTYPE)0;
    m_value1 = 0;
    m_type2 = (ENUM_HACKTYPE)0;
    m_value2 = 0;
    m_value3 = 0;
    m_value4 = 0;
    m_limitCnt = 0;
}

inline HackTypeInfo_t::HackTypeInfo_t(ENUM_HACKTYPE type1, unsigned short value1,
                                      ENUM_HACKTYPE type2, unsigned short value2,
                                      unsigned short value3, unsigned short value4,
                                      unsigned int limitCnt)
{
    m_type1 = type1;
    m_value1 = value1;
    m_type2 = type2;
    m_value2 = value2;
    m_value3 = value3;
    m_value4 = value4;
    m_limitCnt = limitCnt;
}

inline void HackTypeInfo_t::CopyStruct(const HackTypeInfo_t& other)
{
    m_type1 = other.m_type1;
    m_value1 = other.m_value1;
    m_type2 = other.m_type2;
    m_value2 = other.m_value2;
    m_value3 = other.m_value3;
    m_value4 = other.m_value4;
    m_limitCnt = other.m_limitCnt;
}

#if defined(__GNUC__)
#define WONGWORK_EVENT_ASSERT_CAT2(a, b) a##b
#define WONGWORK_EVENT_ASSERT_CAT(a, b) WONGWORK_EVENT_ASSERT_CAT2(a, b)
#define WONGWORK_EVENT_STATIC_ASSERT_SIZEOF(T, N) \
    __extension__ typedef char WONGWORK_EVENT_ASSERT_CAT(__wongwork_event_assert_, __LINE__)[(sizeof(T) == (N)) ? 1 : -1]
#endif

#ifdef WONGWORK_EVENT_STATIC_ASSERT_SIZEOF
WONGWORK_EVENT_STATIC_ASSERT_SIZEOF(HackTypeInfo_t, 0x10);
#endif

// ----------------------------------------------------------------------------
// WebEvent 类事件落库/派发静态接口族。SQL 与控制流依据各 class_func_reports。
// 全部为 static（调用点无 this，mangled 无 const/参数对象）。
// ----------------------------------------------------------------------------

// WongWork::EventAlttle（event_alttle_info 表）
class EventAlttle
{
public:
    static void UpdateEventAlttleInfo(unsigned int uid, int itemNo);
                                           // ORIG T 0x083fb530
    static void ArrangeAlttleEventItem(unsigned int mid, unsigned int serverId);
                                           // ORIG T 0x083fb595
    static int CheckAlreadyGetAlttleItem(unsigned int uid, int itemNo);
                                           // ORIG T 0x083fb472
};

// WongWork::EventSigong（event_sigong_info 表，item1/item2 双栏）
class EventSigong
{
public:
    static void UpdateEventSigongInfo(unsigned int id, int itemNo);
                                           // ORIG T 0x083fa7a5
    static void ArrangeSigongEventItem(unsigned int mid, unsigned int serverId);
                                           // ORIG T 0x083fa855
    static int CheckAlreadyGetSigongItem(unsigned int id, int itemNo);
                                           // ORIG T 0x083fa69c
};

// WongWork::EventEverland（event_everland_info 表，coin 栏）
class EventEverland
{
public:
    static void UpdateEventEverlandInfo(unsigned int id, int itemNo);
                                           // ORIG T 0x083face3
    static void ArrangeEverlandEventItem(unsigned int mid, unsigned int serverId);
                                           // ORIG T 0x083fad60
    static int CheckAlreadyGetEverlandItem(unsigned int id, int itemNo);
                                           // ORIG T 0x083fac0d
};

// WongWork::EventFCCombo（event_combo_entry 表）
class EventFCCombo
{
public:
    static MySQL* GetDBHandle();          // ORIG W 0x0844dc7c
    static void UpdateFCComboEventInfo(unsigned int id, int itemNo);
                                           // ORIG T 0x083fb929
    static void ArrangeFCComboEventItem(unsigned int mid, unsigned int serverId);
                                           // ORIG T 0x083fb976
    static int CheckAlreadyFCComboEventItem(unsigned int id, int itemNo);
                                           // ORIG T 0x083fb883
};

// WongWork::EventCommon（charac_event_items 表公共读写）
class EventCommon
{
public:
    static unsigned int InsertEventItem2EventTable(unsigned int characNo,
                                                   unsigned int itemId,
                                                   unsigned int stackCount,
                                                   unsigned int eventCode);
                                           // ORIG T 0x083f9b54
    static void LoadCharacEventItem(unsigned int eventId,
                                    unsigned int characNo,
                                    unsigned int loadId);
                                           // ORIG T 0x083f9c7b
};

// 全局解析函数（ORIG T 0x08483e98, size 0x115）：按分隔符集切分字符串到 vector。
void arad_parsing(const std::string& src,
                  std::vector<std::string>& out,
                  const std::string& delims);

}  // namespace WongWork

#endif  // DNF_GAME_WONGWORKEVENTS_A_H_
