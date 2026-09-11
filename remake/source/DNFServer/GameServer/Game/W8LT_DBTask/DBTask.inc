// ORIG 0x083fbfc6（报告 docs/class_func_reports/DBTask/DBTask.md）
// [Q5W8LT 2026-09-11 符号名轴落地] 原为类外定义 `const int DBTask::DISPATCHER_COUNT;`
// （C++98 odr-use 保险）—— 实测该定义发射了**发明符号** `_ZN6DBTask16DISPATCHER_COUNTE`
// （R 4B），而 **ORIG 全 ELF 无任何含 `DISPATCHER_COUNT` 的符号**（`nm -S ORIG` 0 命中）。
// 该常量仅在两处作**值使用**（`for (i = 0; i <= DISPATCHER_COUNT - 1; ++i)`，`RegistDispatcher`
// 与 `DBTask::clear` 内），**无取址/引用绑定 ⇒ 非 odr-use**，故定义可删。
// 链接验证（Main 要求的前置）：扫 1429 个 `.o` 的 `nm -u` ⇒ **UND 引用 = 0**；
// 整库 `build/game/df_game_r` 中该符号仅 1 处 R（= 本 TU 定义被链接进去，非引用）。
// overlay 实测：删除后 OUR-only 1→0、四数 143/1771/9/48/705 逐项不变。
DBTask::DBTask()
