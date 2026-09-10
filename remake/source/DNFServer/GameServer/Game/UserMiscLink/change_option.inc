// [P8S1 2026-09-11 P139] 首参拼写改 typedef 名 `itemIndex_t`（GameTypes.h，
//   = unsigned long，mangled 仍 `…13change_optionEm…` 不变）⇒ 宏
//   __PRETTY_FUNCTION__ 按**定义处**拼写展开，与 ORIG prett 串逐字节一致
//   （探针 /tmp/p8s1/pf.cpp：声明 unsigned long、定义 typedef 名 → 展开 typedef 名）。
bool CRandomOptionItemHandle::change_option(itemIndex_t itemIdx, ENUM_RARITY rarity,
