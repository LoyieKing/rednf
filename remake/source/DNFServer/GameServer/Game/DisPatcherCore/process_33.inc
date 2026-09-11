// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
int DisPatcher_QuestAction::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
