#ifndef DNF_GAME_COMMON_STATISTICS_MGR_H_
#define DNF_GAME_COMMON_STATISTICS_MGR_H_

#include <map>

// ORIG 布局（C1 0x8610474、D1 0x8610412，均 W）：vptr +0x00、map +0x04，
// sizeof 0x1c（派生类 CCommonStatisticsMgr::C1 0x86104e8 中 operator new(0x1c)）。
// ORIG vtable 每类仅 1 个虚槽（InsertDatabase 纯虚）；析构非虚。
class CStatisticContainer
{
public:
    virtual void InsertDatabase() = 0;
    ~CStatisticContainer();                // ORIG D1/D2 0x08610412 W（非虚）

    void IncreaseQuantity(int quantity);   // ORIG 0x0860e7f8

protected:
    std::map<int, int> m_quantityMap;      // +0x04
    // ORIG 布局：vptr@0、map@4，总 0x1c。本重建工具链
    // sizeof(std::map<int,int>)==0x18（GCC4.4 32位 ABI），4+24=0x1c
    // 与 ORIG 总尺寸恰好一致，无需额外填充。
};

class CHelpedPartyCount : public CStatisticContainer
{
public:
    CHelpedPartyCount() {}
    void InsertDatabase();                 // ORIG 0x0860e8aa T（pool 0x512, msg 0x103, 键 0..4）
};

class CPVPPlayerRate : public CStatisticContainer
{
public:
    CPVPPlayerRate() {}
    void InsertDatabase();                 // ORIG 0x0860e9e0 T（pool 0x525, msg 0x104, 键 0..5）
};

class CPVPMapCount : public CStatisticContainer
{
public:
    CPVPMapCount() {}
    void InsertDatabase();                 // ORIG 0x0860eb16 T（pool 0x538, msg 0x105, 遍历整表）
};

// ORIG：无 dtor 符号之外的基类依赖；~CCommonStatisticsMgr ORIG 0x08610432 W
// （循环 i<=2 delete 三容器），GetInstance 单例经 __cxa_atexit 注册它。
class CCommonStatisticsMgr
{
public:
    enum ENUM_STATISTIC_CONTAINER
    {
        ENUM_STATISTIC_CONTAINER_0 = 0,
        ENUM_STATISTIC_CONTAINER_1 = 1,
        ENUM_STATISTIC_CONTAINER_2 = 2
    };
    CCommonStatisticsMgr();   // ORIG 0x86104e8 W
    ~CCommonStatisticsMgr();  // ORIG 0x08610432 W

    void IncreaseQuantity(ENUM_STATISTIC_CONTAINER container, int quantity);
    void InsertDatabase();

    CStatisticContainer* m_container[3];
};

// ORIG 0x860e765 T（_Z30GetInstanceCommonStatisticsMgrv）：函数内静态单例，
// __cxa_guard 构造-on-first-use + __cxa_atexit(~CCommonStatisticsMgr)。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr();

#endif  // DNF_GAME_COMMON_STATISTICS_MGR_H_
