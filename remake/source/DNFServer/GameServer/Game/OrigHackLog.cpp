// ============================================================================
// OrigHackLog.cpp — DB_SaveClientHackLog 域（4 个 ORIG 强符号）
//
// 报告来源（逐份转写，汇编+反编译 C）：
//   docs/class_func_reports/DB_SaveClientHackLog/
//     _getTodaysHackCount  T 0x08421d50 _ZN20DB_SaveClientHackLog19_getTodaysHackCountEP5MySQLjt
//     _sendResult          T 0x08421e3a _ZN20DB_SaveClientHackLog11_sendResultEj
//     dispatch             T 0x08421f16 _ZN20DB_SaveClientHackLog8dispatchEiiP6Stream
//     _sendHackTypeResult  T 0x08422152 _ZN20DB_SaveClientHackLog19_sendHackTypeResultEjii
//
// 2026-08-29 迁移整合：类声明与 4 个函数定义已迁入 W8LT_DBTask.cpp
// （唯一权威承载，避免本 TU 与 W8LT_DBTask 的类声明 ODR 冲突及重复定义
// 链接错误；ORIG vtable @0x8c5f5c8 槽序 {dispatch, ~D1, ~D0} 由
// W8LT_DBTask.cpp 的虚覆盖声明保证）。本文件保留（CMake 引用不摘除），
// 编译为空 TU；如需再次拆分请从 W8LT_DBTask.cpp 迁回。
// ============================================================================
