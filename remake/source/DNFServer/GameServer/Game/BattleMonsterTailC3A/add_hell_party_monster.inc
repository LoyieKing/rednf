// [P8S5 2026-09-11] P139 prett 缺口（ORIG 有 / 我方无，REF 0x8307389）：ORIG 串
//   `void CBattle_Field::add_hell_party_monster(map_monster&, int&, int&, MAP_HELL_PARTY_GROUP_ORDER*)`
//   size 0x62@0x08c23700；我方定义第 4 参拼作 `std::map<unsigned short, STHellPartyGroupOrder>*`，
//   直接换 __PRETTY_FUNCTION__ 会发射展开形 ⇒ 需先补 typedef MAP_HELL_PARTY_GROUP_ORDER
//   （**头文件改动，越界：只报告**）⇒ 未改。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void CBattle_Field::add_hell_party_monster(map_monster& mob, int& uid, int& seq,
