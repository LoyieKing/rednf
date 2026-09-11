// b28d-solo-ARAD verdict=IDENTICAL/AE：P96/P270 非 const iterator（消 const_iterator
// 转换构造 call）+ P0 具名 key（ORIG 0x81a2535 `mov %eax,-0x1c`）+ P247 具名 Data tmp
// 后结构整体赋值（ORIG 0x81a2590 逐字段读节点到 -0x14/-0x10/-0xc，再 memcpy(out,&tmp,10)
// 带自赋值检查）。槽位 key -0x1c / find -0x20 / end -0x18 / tmp -0x14 全对齐。
bool AradBonusItem::find(unsigned int itemId, Data& out)
{
    // ORIG 0x81a252c：map<int,Data> 命中写 out（拷 10 字节）返回 true。
    int key = itemId;   // ORIG 具名 key 槽（-0x1c）
    std::map<int, Data>::iterator it = m_bonusMap.find(key);
    if (it != m_bonusMap.end())
    {
        out = Data(it->second);
        return true;
    }
    return false;
}