dword CDataManager::GetMoneyLimitPerLevel(int level, const char* serverGroup) const
// [P8S4 2026-09-11 负结论 · .h 项，只报告] P139 size 不等：
// ORIG prett 串 = "dword CDataManager::GetMoneyLimitPerLevel(int, const char*) const"
// （size 0x42 @0x08c34300，REF 0x08360d0e）；我方 .inc/cpp 手写串仅为
// "CDataManager::GetMoneyLimitPerLevel"（缺返回类型与参数表）⇒ 运行期日志函数名文本不符。
// 修法需：① 新增 `typedef unsigned int dword;`（本源码树无此定义，ORIG 头专有）；
// ② 改 CDataManager.h:3579 声明 + .inc 定义首行拼写为 dword；③ 站点改
// __PRETTY_FUNCTION__。①② 属共享头（.h）越界 ⇒ 本波只报告，未改。
// [Q6HeaderTypeAxis 2026-09-11 已修 · 复核 P8S4 记录正确] ORIG 串确为 `dword …`
//   （0x08c34300 首 5 字节 64 64 77 6f 72 = "dword"，size 0x42）⇒ P8S4 的 ①②③ 修法正确：
//   ① `typedef unsigned int dword;` 已加入 GameTypes.h:69（全库无既有 `dword` 类型定义）；
//   ② CDataManager.h:3631 声明 + 本行定义拼写已改 `dword`（同底层，mangled 不变）；
//   ③ 站点（CDataManager.cpp:466）已改 `__PRETTY_FUNCTION__`，并同轮修正另两处串盲区：
//      fmt `"Out of range level (%d)"` → `"Invalid Character Level %d"`（ORIG 0x08c32e6e）、
//      strcmp 目标 `"0"` → `"rksekrlsek"`（ORIG 0x08c32e89，**语义差**）。
{
    if (level <= 0 || level > 200)
    {
        LogManager::logFormat(1, "data_manager.cpp",
                              // [Q6HeaderTypeAxis 2026-09-11 §6.5 串盲区 + prett 缺口]
                              // ORIG 0x08360d0e：0x4=0x08c2f90a "data_manager.cpp"、
                              // 0x8=0x08c34300 prett、0xc=0x2705(9989)、0x10=0x08c32e6e
                              // "Invalid Character Level %d"、vararg=level。
                              __PRETTY_FUNCTION__, 0x2705,
                              "Invalid Character Level %d", level);
        level = std::max(1, std::min(200, level));
    }
    // [Q6HeaderTypeAxis 2026-09-11 语义轴·串] ORIG 0x08360d64 `movl $0x8c32e89,0x4(%esp)`
    // ⇒ 比较串实为 "rksekrlsek"（.rodata 手读：72 6b 73 65 6b 72 6c 73 65 6b 00），
    // 我方原写 "0"（串盲区，行为不同：该 serverGroup 才返回 INT_MAX）。
    if (serverGroup && strcmp(serverGroup, "rksekrlsek") == 0)
    {
        return 0x7fffffff;
    }
    return m_moneyLimit[level];
}