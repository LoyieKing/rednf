// [K6 2026-09-11·P292 档①] int→bool（配合 CDataManager.h:3645）：ORIG 调用点
// 0x081c6b30 `call; test %al,%al; je`（裸 bool 直判）；定义体各分支为常量 return 0/1，
// 4.4 探针实测 bool 与 int 形态全等（mov $0/%eax）⇒ 体机器码不变。
bool CDataManager::hasPreventPrefixCharacName(const char* name, int /*unused*/)
