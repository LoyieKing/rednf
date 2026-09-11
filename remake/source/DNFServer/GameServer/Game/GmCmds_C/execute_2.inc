// ORIG execute 0x084abdb4：CAccountCargo::DeleteAccountCargo(GetUser())。
// ORIG 该函数为静态语义直调（cdecl，无 this 使用）；此处经 CUser 内嵌实例
// （布局注记 CUser.h：+0x6effc CAccountCargo）走同一实现路径。
void CDeleteAccountCargo::execute()
{
    // ORIG：GetUser 结果经 eax 直通 DeleteAccountCargo 实参槽（无 user/cargo 具名槽）。
    CAccountCargo::DeleteAccountCargo(GetUser());
}