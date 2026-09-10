void* AccountCargoScript::GetCreateInfo()
{
    // ORIG 0x88c806c begin()→临时 -0x10；0x88c8084 end()→临时 -0xc；
    // 0x88c8099 ne(&it,&end)：无 empty() 调用。命中 0x88c80ad add $4 直返；
    // je→0x88c80b2 mov$0 尾岛。
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.begin();
    if (it != m_upgradeMap.end())
        return &it->second;
    return 0;
