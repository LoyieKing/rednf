// [b28d-solo-DisPatcher_DebugCommand_cmds] AE（4.4.6 后端路由已落，Main 2026-09-09）：
// IsAvatarItem 间接 call 目标 %edx 与 ORIG 同形（见 AddItem 判词）。已修：
// `slot != -1` 正向包裹翻失败出口块序（P222）、usablePeriod 内联（P227）、
// G_CEnvironment 具名字段。P167 callsite CLEAR 18/18。
bool DisPatcher_DebugCommand::_debugCommandAddAvatar(CUser* user,
