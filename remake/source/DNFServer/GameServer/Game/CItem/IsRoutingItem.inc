// ORIG 0x8150f18（weak）：movzbl 0xbd(%eax) — 返回 +0xbd 路由标志
bool CItem::IsRoutingItem() const
{
    return m_isRoutingItem;
}