// ============================================================================
// DB_LoadEtc::LoadQuestShop @0x08404a68 —— 主/tag 双实例装载。
// 主区 = SIG_LOAD_ETC+0xd5a7，tag 区 = +0xd651（tag_charac_no=+0xc 非零时装载），
// 双区齐备后 +0xd6fb 置 1。错误走 cMyTrace 0x16c4/0x16d5。
// ============================================================================
// [b28d-solo-DB_LoadEtc] 旧判「N封顶（N107 成员常量偏移取址）」已作废（b29-Lane44 推翻）：
//   ORIG 两处 `lea 0xd5a7/0xd651(%eax),%edx` 对应**视图成员取址**形；旧探针用「pad+char 数组
//   退化」形（char→CHARAC_QUEST_SHOP_DATA* 仍需 cast）故见 add+mov。改用局部 trivial 镜像
//   视图 `QuestShopView` 的具名对象成员取址（类型匹配、无 cast）后 4.4 产单条 lea ⇒ 69/69 全等。
// 已修（前波）：exec 具名 ok + trace 临时链（P227）。
bool DB_LoadEtc::LoadQuestShop(SIG_LOAD_ETC* sig)
