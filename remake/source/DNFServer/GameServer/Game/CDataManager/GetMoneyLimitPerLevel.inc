unsigned int CDataManager::GetMoneyLimitPerLevel(int level, const char* serverGroup) const
// [P8S4 2026-09-11 负结论 · .h 项，只报告] P139 size 不等：
// ORIG prett 串 = "dword CDataManager::GetMoneyLimitPerLevel(int, const char*) const"
// （size 0x42 @0x08c34300，REF 0x08360d0e）；我方 .inc/cpp 手写串仅为
// "CDataManager::GetMoneyLimitPerLevel"（缺返回类型与参数表）⇒ 运行期日志函数名文本不符。
// 修法需：① 新增 `typedef unsigned int dword;`（本源码树无此定义，ORIG 头专有）；
// ② 改 CDataManager.h:3579 声明 + .inc 定义首行拼写为 dword；③ 站点改
// __PRETTY_FUNCTION__。①② 属共享头（.h）越界 ⇒ 本波只报告，未改。
