// ---------------------------------------------------------------------------
// CItemUpgrade::find3rdChronicleEnchantData —— ORIG T 0x0854a916。
// itemScript=CDataManager::find_item(typeIdx)；空→0；map@item+0x2f4 查 key，
// 未命中→0；命中返回 &it->second（Ghidra 报告 node+0x10 语义即 second 地址）。
// [b28d-solo-WongWorkMiscB_A] ①去 `EnchantMap&` 具名引用（P0/P183：ORIG 无引用槽，
// find/end 各自重算 `mov itemScript,%eax; lea 0x2f4(%eax),%ecx`）；②+0x2f4 改用
// 权威成员 `CStackableItem::m_enchantMap`（§4.3 去裸偏移）；③条件改正极性
// `if (it != end) return &it->second; return 0;`（P220/P47：ORIG `je` 落失败尾块）。
STEnchantSystemMapData* CItemUpgrade::find3rdChronicleEnchantData(
