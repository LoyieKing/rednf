// ORIG 0x0812bf16 T _ZN33DisPatcher_EventDungeon_ClearRoom4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/send.md 全文转写）：
// put_header(1,0x21c) → put_byte(1) → put_binary(pParam,0xc) → finalize(true) →
// CUser::Send；PacketGuard 栈对象析构（ORIG 含 EH 清理路径）。
// 反编译 C 首行 local_10=param_2：param 引用先落栈（-0xc），guard 位于 -0x18。
void DisPatcher_EventDungeon_ClearRoom::send(CUser* user, ParamBase& param)

{
    ParamBase* pParam = &param;   // ORIG -0xc(%ebp)（local_10，先于 guard ctor 落栈）
    PacketGuard guard;
    guard.put_header(1, 0x21c);
    guard.put_byte(1);
    guard.put_binary(reinterpret_cast<char*>(pParam), 0xc);
    guard.finalize(true);
    user->Send(guard);
}
