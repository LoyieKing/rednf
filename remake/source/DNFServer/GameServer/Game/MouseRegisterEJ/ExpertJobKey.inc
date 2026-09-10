CExpertJobStatistic::ExpertJobKey::ExpertJobKey(ENUM_EXPERT_JOB_TYPE type,
                                                unsigned long itemIdx)
{
    // P95：ORIG 0x8610720 首句先载 this 后载 type（enum 参数）；4.4 直初始化值先。
    // 具名视图复现（m_field00 = +0x00）。
    struct EJKeyView
    {
        int m_field00;
        unsigned long m_itemIdx04;
    };
    ((EJKeyView*)this)->m_field00 = type;
    ((EJKeyView*)this)->m_itemIdx04 = itemIdx;
