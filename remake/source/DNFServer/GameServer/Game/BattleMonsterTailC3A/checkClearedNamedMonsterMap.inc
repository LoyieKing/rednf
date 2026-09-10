// [P8S5 2026-09-11 负结论·4.4 bool 值域折叠] 唯一残差（tail，析构后）：
//   ORIG 0x8300d91 `test %esi,%esi; je 0x8300d9a`；我方 0x3e9 `cmp $0x1,%esi; jne`。
//   %esi 两处常量赋值（`mov $0x1,%esi` / `mov $0x0,%esi`）⇒ 4.4 知值域{0,1} 把 `!=0` 折成 `==1`；
//   4.1 不折。条件表达式形（`isNamedMonster()!=0` → `isNamedMonster()`）已同形（`test %al,%al; je`），
//   尾部折叠点在源码之外（编译器 fold）⇒ 不可造。本波未改（gap −1、blocks 1）。
bool CBattle_Field::checkClearedNamedMonsterMap(unsigned char idx, int uid)
