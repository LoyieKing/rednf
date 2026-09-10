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
