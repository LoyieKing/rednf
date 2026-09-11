// ----------------------------------------------------------------------------
// ProcessIPG_ResultInput —— ORIG 0x0817866c（0x9 字节：push/mov/cmp 0xc(%ebp)/pop/ret，
// 判空无副作用，函数体忠实为空）。
// ----------------------------------------------------------------------------
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
}