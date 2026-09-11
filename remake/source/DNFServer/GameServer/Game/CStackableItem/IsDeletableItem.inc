// ORIG 0x822ca48 (W)：movzbl 0x321(%eax),%eax —— 直接返回 bool 字段（无 test/setne）。
bool CStackableItem::IsDeletableItem() const
{
    return m_char321;
}