// [P8S5 2026-09-11] P139 prett 缺口（ORIG 有 / 我方无，REF 0x83035e9+0x830363d）：
//   ORIG 串 `bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)`
//   size 0x6a@0x08c23960；我方手写串与其逐字节相同，但定义签名拼作 `std::list<_mapMonster>&` /
//   `std::map<short, std::vector<TournamentInfo> >&` ⇒ 换 __PRETTY_FUNCTION__ 会发射展开形（≠ ORIG）。
//   需先补 typedef MOB_REF_LIST / CBattle_Field::TournamentRoundInfoMap（**头文件改动，越界：只报告**）⇒ 未改。
// [prett-A 2026-09-11] **已落地**（R2PrettAClass 授权 + 探针实证；原「需头文件改动 ⇒ 维持」判断作废）：
//   ① .cpp 文件作用域（非 namespace）加 `typedef std::list<_mapMonster> MOB_REF_LIST;`（宏打裸名）；
//   ② CBattle_Field.h 类内**纯增量**加 `typedef std::map<short, std::vector<TournamentInfo> > TournamentRoundInfoMap;`
//      —— GCC 4.4 探针（/tmp/r2/probe2.cpp）实证：类内 typedef 形参在类外定义 ⇒ 宏打印**全限定名**；
//   ③ 定义侧两处拼写改 typedef 名（同类型 ⇒ mangled 不变）；
//   ④ 三处 cMyTrace 手写串 → `__PRETTY_FUNCTION__`（删字面量、调用点内联）。
//   ORIG 符号 `_ZZN13CBattle_Field17ConsistTournamentEPK4CMapRSt4listI11_mapMonsterSaIS4_EERSt3mapIsSt6vectorINS_14TournamentInfoESaISA_EESt4lessIsESaISt4pairIKsSC_EEEE19__PRETTY_FUNCTION__`
//   @0x08c23960 size 0x6a，串 `bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)`。
//   ⚠ 宏为整串匹配 ⇒ 三处调用点必须同轮改（任一残留旧串即发射不匹配文本）。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
bool CBattle_Field::ConsistTournament(const CMap* pMap, MOB_REF_LIST& mobList,
