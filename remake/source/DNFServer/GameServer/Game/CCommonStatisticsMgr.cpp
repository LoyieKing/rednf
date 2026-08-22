// ============================================================================
// df_game_r 还原 —— CCommonStatisticsMgr（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/CCommonStatisticsMgr.md + ORIG 反汇编。
// 布局：+0x00 CStatisticContainer* m_container[3]。
// ============================================================================
#include <stddef.h>

class CStatisticContainer
{
public:
    virtual void InsertDatabase();
    void IncreaseQuantity(int quantity);
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

    CCommonStatisticsMgr();

    void IncreaseQuantity(ENUM_STATISTIC_CONTAINER container, int quantity);
    void InsertDatabase();

    CStatisticContainer* m_container[3];  // +0x00
};

CCommonStatisticsMgr::CCommonStatisticsMgr()
{
    // ORIG ctor 0x86104e8：依次 new CHelpedPartyCount/CPVPPlayerRate/... 并
    // 存入 m_container。子容器类型属统计域后续批次，此处先清零防未初始化。
    m_container[0] = NULL;
    m_container[1] = NULL;
    m_container[2] = NULL;
}

void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0) return;
    if (container > 2) return;
    if (m_container[container] == NULL) return;
    m_container[container]->IncreaseQuantity(quantity);
}

void CCommonStatisticsMgr::InsertDatabase()
{
    for (int i = 0; i <= 2; ++i)
    {
        if (m_container[i] == NULL) continue;
        m_container[i]->InsertDatabase();
    }
}

// ORIG 0x860e765 T（_Z30GetInstanceCommonStatisticsMgrv）：函数内静态单例，
// __cxa_guard 构造-on-first-use + __cxa_atexit 析构；实例位于 ORIG BSS
// 0x94733ac。PvP_Room.cpp 经 sub_GetInstanceCommonStatisticsMgr 直引。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr()
{
    static CCommonStatisticsMgr instance;
    return &instance;
}
