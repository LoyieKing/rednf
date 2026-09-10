bool PurchaseLimitItem::find(unsigned int itemIndex, Info& out)
{
    // ORIG 0x81a2310：map<uint,Info> 命中写 out 返回 true。
    // P96：4.4 的 const_iterator 与 iterator 是不同类型，find 返回值赋 const_iterator
    // 会多一次转换构造（ORIG 无该 call）；ORIG 侧 cv 限定为非 const iterator。
    std::map<unsigned int, Info>::iterator it = m_purchaseLimitMap.find(itemIndex);
    // P222：ORIG 0x81a235f 为正向包裹（`test; je <mov $0 失败岛>; 命中体; mov $1`），
    // 否定早返 `if (it == end) return false;` 产反向块序。
    if (it != m_purchaseLimitMap.end())
    {
        out = it->second;
        return true;
    }
    return false;
