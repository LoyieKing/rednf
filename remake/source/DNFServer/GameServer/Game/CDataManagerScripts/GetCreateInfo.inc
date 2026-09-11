// ============================================================================
// AccountCargoScript 查询（ORIG 0x88c8056 / 0x88c80ba / 0x88c8124）。
// 全部基于 +0x8 std::map<int, UpgradeInfo>：
//   GetCreateInfo：begin() 的 value（首个升级档），空 map 返回 0；
//   GetCurrUpgradeInfo(idx)：find(idx) 的 value，未找到返回 0；
//   GetNextUpgradeInfo(idx)：find(idx) 的下一项 value，无下一项返回 0。
// 返回 void* 以匹配 CAccountCargo.cpp 的 asm 别名调用。
// ============================================================================
void* AccountCargoScript::GetCreateInfo()
{
    // ORIG 0x88c806c begin()→临时 -0x10；0x88c8084 end()→临时 -0xc；
    // 0x88c8099 ne(&it,&end)：无 empty() 调用。命中 0x88c80ad add $4 直返；
    // je→0x88c80b2 mov$0 尾岛。
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.begin();
    if (it != m_upgradeMap.end())
        return &it->second;
    return 0;
}