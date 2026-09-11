// ORIG 0x822ca28：movzwl 0x2b4(%eax) —— 返回珍宝目标槽位（16 位，单次零扩展）
unsigned short CStackableItem::getJewelTargetSocket() const
{
    return m_jewelTargetSocket;
}