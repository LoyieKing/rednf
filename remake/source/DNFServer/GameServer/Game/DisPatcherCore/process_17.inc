// ORIG 0x08232f70 W _ZN19DisPatcher_SendMess7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部聊天业务（type 分派/表情改写/敏感词 Filter/消耗品扣减/community+monitor 监视包/
// DB_InsertPoliceSaveChatting/GameWorld::send_chat_msg）内嵌于
// dispatch_sig(0x081f3540，报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md)。
int DisPatcher_SendMess::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
