void CDeleteAccountCargo::execute()
{
    // ORIG：GetUser 结果经 eax 直通 DeleteAccountCargo 实参槽（无 user/cargo 具名槽）。
    CAccountCargo::DeleteAccountCargo(GetUser());
