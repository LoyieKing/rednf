Inter_MailBox_Req_CheckReceiver::~Inter_MailBox_Req_CheckReceiver() {}
// ORIG 0x084cf95a T dispatch_sig —— 邮箱查验收件人请求：按检查类型（普通邮件 /
// cera 直购 / 多附件邮件）完成校验、购买或入 DB 线程，并回 ACK 错误包。
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/dispatch_sig.md 全文转写。
// SendErrorPacket/SendDBThread_forchina 为本类既有方法（NEEDS_DECL 见 meta）。
// [b28d-lane2] verdict=DIFF ndiff=373（ORIG 0x084cf95a）：槽序/帧差(N82/P212/P221)+混合残差；gap-9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=303（ORIG 0x84cf95a）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
