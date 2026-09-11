// ORIG 0x0811cb34：忽略参数；置标志；trace 行号 46。
// 报告反编译：StartEvent(Word_Param) 与无参版完全相同（参数未使用），
// 汇编仅 SetEventFlag + cMyTrace 两段，无字段写入（该类无派生字段）。
// 与 StartEvent() 的唯一差异是行号（0x2e vs 0x20）与 __FUNCTION__ 符号。
// 参数按值传入（Word_Param 4 字节），无成员写入。
// 行号 0x2e=46（ORIG movl $0x2e,0x8(%esp)）。
// param 未参与任何运算（ORIG 无 movzwl 读参）。
// Word_Param 按值传参（4 字节），汇编仅 SetEventFlag 段后即 trace。
void CFirstServerOpenEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 46, 0);
    trace("First Server Open Event Start!");
}