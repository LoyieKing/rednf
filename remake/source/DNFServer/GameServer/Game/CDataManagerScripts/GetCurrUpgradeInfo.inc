void* AccountCargoScript::GetCurrUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    // ORIG 0x88c80f1 ne + 命中 fall-through；je→0x88c811d mov$0 尾岛
    if (it != m_upgradeMap.end())
        return &it->second;
    return 0;
}