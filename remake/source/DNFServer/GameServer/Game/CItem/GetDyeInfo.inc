// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
DyeInfo* CItem::GetDyeInfo() const
{
    return const_cast<DyeInfo*>(&m_dyeInfo9c);
}