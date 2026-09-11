// ORIG 0x8694736：movzbl 0x2e7(%eax); ret —— 直接返回 bool 字段（无 test/setne 归一）。
bool CStackableItem::IsExpAffect()
{
    return m_char2e7;
}