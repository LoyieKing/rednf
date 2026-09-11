// ---- CDBAndExchangeServerProc（APSystemTypes.h 声明；Prod_GlobalAdv 2026-08-24）----
// ORIG T 0x08121608 _ZN8APSystem24CDBAndExchangeServerProc33RequestLoadActionPointFromDBAndESER5CUserR17MSG_CHARAC_SELECT
//（docs/class_func_reports/APSystem__CDBAndExchangeServerProc/ 全文已读）：
// ORIG 体 13 字节 push/mov/sub + mov $0x1,%eax + leave/ret —— 忠实转写 return true。
bool CDBAndExchangeServerProc::RequestLoadActionPointFromDBAndES(CUser&, MSG_CHARAC_SELECT&)
{
    // [b28c-4] ORIG 0x08121608：sub $0x10 + mov $1,%eax ⇒ 空体带未使用局部帧。
    char framePad[0x10];
    (void)framePad;
    return true;
}