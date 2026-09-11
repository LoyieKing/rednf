// [RentalPrett 2026-09-11 · 负结论 / 维持手写串] 本件**无法**换 `__PRETTY_FUNCTION__`：
//   ORIG prett 符号 `_ZZN13random_option23CRandomOptionItemHandle20RegenVer_choose_item
//   E11ENUM_RARITYiRKSt6vectorIiSaIiEEE19__PRETTY_FUNCTION__` @0x8cd10a0 size=0x8a，
//   串 = `itemIndex_t random_option::CRandomOptionItemHandle::RegenVer_choose_item(
//   ENUM_RARITY, int, const std::vector<int, std::allocator<int> >&)`。
//   宏按定义处拼写打印**返回类型** ⇒ 定义侧须写 `itemIndex_t`(=unsigned long)，
//   与类内声明 `unsigned int`（CItemGenerator.h:446）**类型不同** ⇒ 实测编译错误
//   `RegenVer_choose_item.inc:1: prototype for 'itemIndex_t …' does not match any in class
//   'random_option::CRandomOptionItemHandle'（candidate is: unsigned int …）`。
//   修它必须改 CItemGenerator.h（本任务禁止改 .h）⇒ 保留 .cpp 手写串。
unsigned int CRandomOptionItemHandle::RegenVer_choose_item(
