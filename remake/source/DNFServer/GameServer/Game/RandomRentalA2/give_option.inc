// [RentalPrett 2026-09-11] 首参拼写改 typedef 名 `itemIndex_t`（GameTypes.h，= unsigned long；
//   mangled 仍 `…11give_optionEm…` 不变）⇒ __PRETTY_FUNCTION__ 按**定义处**拼写展开，
//   与 ORIG prett 串 `bool random_option::CRandomOptionItemHandle::give_option(itemIndex_t,
//   ENUM_RARITY, int, int, int, RandomOption*)` 逐字节一致（同 TU 先例：change_option P139）。
bool CRandomOptionItemHandle::give_option(itemIndex_t itemIdx,
