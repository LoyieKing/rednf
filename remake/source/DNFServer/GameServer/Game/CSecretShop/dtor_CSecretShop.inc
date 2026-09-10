CSecretShop::~CSecretShop()
{
    // ORIG 顺序：先 ::operator delete(m_pRand)（_ZdlPv），再遍历 map 用
    // ::operator delete 释放每个无 virtual 析构的 IBuyRule 指针，
    // m_rules/m_statistic 的析构由成员声明顺序自动完成。
    if (m_pRand != 0)
        ::operator delete(m_pRand);
    for (std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.begin();
         it != m_rules.end(); ++it)
        ::operator delete(it->second);
    m_rules.clear();
