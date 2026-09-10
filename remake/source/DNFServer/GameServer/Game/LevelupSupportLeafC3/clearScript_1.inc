void LevelupSupport2ndScript::clearScript()
{
    std::map<int, std::vector<ItemInfo>*>::iterator it = m_stepItems.begin();
    std::map<int, std::vector<ItemInfo>*>::iterator endIt = m_stepItems.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08ab59c6：`call ptEv; mov 0x4(%eax),%ebx; test %ebx,%ebx;
        // je`（无槽往返，P0）⇒ 直接 delete it->second，编译器自带空指针守卫（P141）。
        delete it->second;   // 不 ++it（忠实）
    }
    m_stepItems.clear();
