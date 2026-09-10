// [P8S6 2026-09-11] P139 修复：ORIG prett 串（0x08c23b60 size 0x85）=
// `void CBattle_Field::add_monsters_current_map(MOB_REF_LIST&, std::vector<int, ...>&, ...)`
// ⇒ 第 1 参按 ORIG 拼写 MOB_REF_LIST&（statically identical to std::list<_mapMonster>，
// mangled 不变、零 ABI/零 codegen）。typedef 落本 include 位置（全局作用域 ⇒ prett 打裸名）。
typedef std::list<_mapMonster> MOB_REF_LIST;
void CBattle_Field::add_monsters_current_map(MOB_REF_LIST& mobList,
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
