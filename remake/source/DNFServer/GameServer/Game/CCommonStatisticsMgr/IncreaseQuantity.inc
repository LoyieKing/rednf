// ORIG 0x0860e7f8 T：先经 operator[] 取值入局部，再二次 operator[]
// 以 ++value 写回（保持 ORIG 的双调用与栈临时增量形态）。
void CStatisticContainer::IncreaseQuantity(int quantity)
{
    int value = m_quantityMap[quantity];
    m_quantityMap[quantity] = ++value;
}