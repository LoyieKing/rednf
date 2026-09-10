CItemLimitEdition::CItemLimitEdition(const stItemLimitEditionItemInfo_t& info)
{
    // [b28c-L18] P83：ORIG 0x084e9732 为 `rep movsl`×0x12（结构整体赋值展开形），
    // 4.4 对 memcpy 恒 call memcpy（N78）；改同布局具名结构整体赋值（前 0x48 字节
    // 布局一致，头文件注释实证）即复现块搬移 + info+0x18 写 this+0x48（初始销量）。
    *reinterpret_cast<stItemLimitEditionItemInfo_t*>(this) = info;
    m_curSellNum = info.m_initSellNum;
