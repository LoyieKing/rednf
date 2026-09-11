// ORIG W 0x08545fda（0x0e）：mov 0xf4(%eax),%eax — 返回 +0xf4 所需等级
int CItem::GetNeedLevel() const
{
    return m_intF4;
}