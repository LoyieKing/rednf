#ifndef DNF_GAME_COMMON_STATISTICS_MGR_H_
#define DNF_GAME_COMMON_STATISTICS_MGR_H_

#include <map>

// ORIG 布局（C1 0x8610474）：vptr +0x00、map +0x04，总 0x1c。
class CStatisticContainer
{
public:
    virtual void InsertDatabase() = 0;
    void IncreaseQuantity(int quantity);   // ORIG 0x0860e7f8

public:
    CStatisticContainer() {}

protected:

    std::map<int, int> m_quantityMap;      // +0x04
    char m_pad10[0x1c - 0x10];             // +0x10..0x1b
    // 三个 InsertDatabase 共用骨架（packetId 0x103/0x104/0x105）。
    void insertDatabaseCommon(int packetId, bool fixedFive);
};

class CHelpedPartyCount : public CStatisticContainer
{
public:
    CHelpedPartyCount() {}
    void InsertDatabase();                 // ORIG 0x0860e8aa
};

class CPVPPlayerRate : public CStatisticContainer
{
public:
    CPVPPlayerRate() {}
    void InsertDatabase();                 // ORIG 0x0860e9e0
};

class CPVPMapCount : public CStatisticContainer
{
public:
    CPVPMapCount() {}
    void InsertDatabase();                 // ORIG 0x0860eb16
};

class CCommonStatisticsMgr
{
public:
    enum ENUM_STATISTIC_CONTAINER
    {
        ENUM_STATISTIC_CONTAINER_0 = 0,
        ENUM_STATISTIC_CONTAINER_1 = 1,
        ENUM_STATISTIC_CONTAINER_2 = 2
    };
    CCommonStatisticsMgr();   // ORIG 0x86104e8

    void IncreaseQuantity(ENUM_STATISTIC_CONTAINER container, int quantity);
    void InsertDatabase();

    CStatisticContainer* m_container[3];
};

// ORIG 0x860e765 T（_Z30GetInstanceCommonStatisticsMgrv）：函数内静态单例。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr();

#endif  // DNF_GAME_COMMON_STATISTICS_MGR_H_
