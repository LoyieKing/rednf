void CAccountCargo::DeleteAccountCargo(CUser* user)
{
    // ORIG 形：`if (!IsExistAccountCargo()) return;`（gcc 对 bool 否定发
    // xor $0x1; test; jne end + 尾 jmp/nop 块布局）。ORIG 还在调 makeRequest 前
    // 取一次 GetAccountCargo 结果入栈槽 -0xc 后不再引用（P0：具名局部只写）。
    if (!user->IsExistAccountCargo())
    {
        return;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    (void)cargo;
    DB_DeleteAccountCargo::makeRequest(user->GetUID(), user->get_acc_id());
    user->DeleteCargo();
