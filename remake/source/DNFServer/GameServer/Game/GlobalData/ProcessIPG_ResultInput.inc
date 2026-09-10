void ItemVendingMachine::ProcessIPG_ResultInput(CUser* user,
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    // ORIG 0x0817866f：`cmpl $0x0,0xc(%ebp)` 后无分支——4.4 对
    // `if (user == 0) return;`（分支目标=相邻 epilogue）做 jump threading，
    // 保留 flag-setting cmp。
    if (user == 0)
        return;
