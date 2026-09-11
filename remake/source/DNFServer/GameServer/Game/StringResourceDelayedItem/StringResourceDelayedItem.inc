//   m_pCache=0 在 CharString 构造之前 ⇒ 须走初始化列表（声明序先于子对象构造）。
StringResourceDelayedItem::StringResourceDelayedItem(StringResource const& res, int id)
    : m_pCache(0)
{
    m_pRes = &res;
    m_id = id;
}