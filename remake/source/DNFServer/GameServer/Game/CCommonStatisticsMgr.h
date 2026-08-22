#ifndef DNF_GAME_COMMON_STATISTICS_MGR_H_
#define DNF_GAME_COMMON_STATISTICS_MGR_H_

#include <map>

class CStatisticContainer
{
public:
    virtual void InsertDatabase() = 0;
    void IncreaseQuantity(int quantity);

protected:
    std::map<int, int> m_quantityMap;
};

class CHelpedPartyCount : public CStatisticContainer
{
public:
    void InsertDatabase();
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

    void IncreaseQuantity(ENUM_STATISTIC_CONTAINER container, int quantity);
    void InsertDatabase();

    CStatisticContainer* m_container[3];
};

// ORIG 0x860e765 T（_Z30GetInstanceCommonStatisticsMgrv）：函数内静态单例。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr();

#endif  // DNF_GAME_COMMON_STATISTICS_MGR_H_
