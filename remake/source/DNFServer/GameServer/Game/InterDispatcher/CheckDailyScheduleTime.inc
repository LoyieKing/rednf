bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);
// break_away_prevent::CBreakAwayPreventSystem —— 唯一权威声明见
// BreakAwayPreventSystem.h（B2 批次与 BAFix 批次共建）。
// DB_LoadCleanPadPoint::makeRequest 仅调用视图（完整类与定义在 GlobalData.cpp；
// makeRequest 为 static，不访问对象成员，静态 ABI 精确匹配 ORIG 符号）。
