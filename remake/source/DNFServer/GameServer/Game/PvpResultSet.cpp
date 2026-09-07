// ============================================================================
// df_game_r PvpResultSet（G2-4 PvP 结果集合，PvpResultType 派生薄封装）
// 类声明见 PvpResultSet.h（唯一声明点）。三个薄封装定义（C1/D1/Clear）已迁至
// PvpResultType.cpp 尾部——ORIG 三件 outgoing sub $0x4 为 4.1 对同 TU callee 的
// 精确分配；callee（PvpResultType C1/D1/Clear）定义在 PvpResultType.cpp，
// unit-at-a-time 要求调用者与 callee 同 TU 才能复现（NL3 实证）。
// ============================================================================
