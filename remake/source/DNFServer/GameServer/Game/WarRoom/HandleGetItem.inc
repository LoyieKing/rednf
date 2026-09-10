// [K6 2026-09-11·P292 档①] int→bool（配合 WarRoom.h:89）：ORIG 调用点 0x081c3cac
// `call; test %al,%al; je 0x081c3cd2` 单字节判定；4.4 探针实测 bool 与 int 的
// `return 0/1` 形态全等（mov $0/%eax）⇒ 定义体机器码不变。
bool WarRoom::HandleGetItem(CUser* user, int idx)
