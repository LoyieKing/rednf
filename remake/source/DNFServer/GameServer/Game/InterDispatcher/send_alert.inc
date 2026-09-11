// ORIG 0x084e5e62 W _ZN17Inter_HadesAction10send_alertEP5CUserj
// 报告 docs/class_func_reports/Inter_HadesAction/send_alert.md：PacketGuard→
// clear→put_header(0,0x1bf)→put_int(code)→finalize(true)→user->Send。
void Inter_HadesAction::send_alert(CUser* user, unsigned int code)
