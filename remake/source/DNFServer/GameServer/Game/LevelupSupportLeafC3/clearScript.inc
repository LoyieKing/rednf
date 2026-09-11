// ============================================================================
// LevelupSupportSciprt::clearScript —— 报告 clearScript.md @0x08ab5442。
// begin/end 遍历 delete 堆上 vector 后 map.clear。注意：ORIG 循环体【无迭代器
// 递增】（汇编 +0x33..+0x55 无 operator++ 调用）——忠实复刻；map 非空时行为与
// ORIG 一致（首元素反复析构路径）。
// ============================================================================
void LevelupSupportSciprt::clearScript()
{
    std::map<int, std::vector<ItemInfo>*>::iterator it = m_stepItems.begin();
    std::map<int, std::vector<ItemInfo>*>::iterator endIt = m_stepItems.end();
    while (it != endIt)
    {
        // [b28c-L29] ORIG 0x08ab5442：ptEv 后 mov 0x4(%eax),%ebx 直接进 delete
        // （无具名 vec 局部槽，P0；delete 自带判空守卫，P141）——不 ++it（忠实）。
        delete it->second;   // ~vector(D1) + operator delete
    }
    m_stepItems.clear();
}