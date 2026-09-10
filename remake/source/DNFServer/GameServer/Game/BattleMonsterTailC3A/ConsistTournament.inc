// [P8S5 2026-09-11] P139 prett 缺口（ORIG 有 / 我方无，REF 0x83035e9+0x830363d）：
//   ORIG 串 `bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)`
//   size 0x6a@0x08c23960；我方手写串与其逐字节相同，但定义签名拼作 `std::list<_mapMonster>&` /
//   `std::map<short, std::vector<TournamentInfo> >&` ⇒ 换 __PRETTY_FUNCTION__ 会发射展开形（≠ ORIG）。
//   需先补 typedef MOB_REF_LIST / CBattle_Field::TournamentRoundInfoMap（**头文件改动，越界：只报告**）⇒ 未改。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
bool CBattle_Field::ConsistTournament(const CMap* pMap, std::list<_mapMonster>& mobList,
