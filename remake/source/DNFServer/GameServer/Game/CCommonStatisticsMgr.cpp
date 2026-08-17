// ============================================================================
// df_game_r 还原 —— CCommonStatisticsMgr（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/CCommonStatisticsMgr.md + ORIG 反汇编。
// 布局：+0x00 CStatisticContainer* m_container[3]。
// ============================================================================

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

    void IncreaseQuantity(ENUM_STATISTIC_CONTAINER container, int quantity);
    void InsertDatabase();

    CStatisticContainer* m_container[3];  // +0x00
};

void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0) return;
    if (container > 2) return;
    m_container[container]->IncreaseQuantity(quantity);
}

void CCommonStatisticsMgr::InsertDatabase()
{
    for (int i = 0; i <= 2; ++i)
    {
        m_container[i]->InsertDatabase();
    }
}
