// [K9GameRestA-S322 · 定义侧同步] 返回类型 int→bool（声明 CDeathTower.h:265 已同步）：
// ORIG 调用点 0x081c3b93 `call; test %al,%al; je 0x081c3cd2`（P292 档① 单字节直判）；
// 返回类型不入 mangling ⇒ 符号/vtable 槽序/sizeof 零变化。
bool CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
