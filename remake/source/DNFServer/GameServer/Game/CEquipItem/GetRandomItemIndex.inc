// ===================== RandomItemTable::GetRandomItemIndex（ORIG 0x089bbb6c，const）=====================
// 遍历 m_vec4（pair<int,int> 数组，+0x04）：找第一个 it->second > index 的元素，
// 返回其 it->first；未命中返回 0。SetEggItem 依赖此把随机权重映射到物品 index。
int RandomItemTable::GetRandomItemIndex(int index) const
{
    // [b28c-21 代落 C28L35 spec] ORIG 增量用后置 ++（operator++(int) 三参形）。
    for (std::vector<std::pair<int, int> >::const_iterator it = m_vec4.begin();
         it != m_vec4.end(); it++)
    {
        if (index < it->second)
            return it->first;
    }
    return 0;
}