// ORIG T 0x08100e0a (0x194)：前置写保留字段后按 Cancel 同款拷 req/res → 投递。
// [b28d-solo-W6_IPGDeps] verdict=AE（103=103）。修法：① 删 dv/rv/sv 具名局部，
// req/res 各点成员链重算（P0/P183）；② tran 拷贝改 union 视图整体赋值（P246）
// ⇒ ORIG 尾部 res 指针单载 + 3 dword 连续拷贝。P167 CLEAR（9/9）。
void CIPGHelper::IPGOutputConfirm(SIPGData* req, SIPGData* res)
{
    SIPGData* d = m_mgr->AllocIPGData();
    // [b28d-solo-W6_IPGDeps] ORIG 0x08100e0a 无 dv/rv/sv 具名中转（d 槽单承载，
    // req/res 各点 `mov 0xc/0x10(%ebp),%eax; add $0xNN` 重算）⇒ 全成员链（P0/P183）。
    d->m_uid = 0xffffffff;                       // ORIG 08100e24 `movl $0xffffffff,0x20`
    d->m_type = (IPG_DATA_TYPE)5;                // ORIG 08100e2e `movl $0x5,0x1c`
    d->m_notifyFlag = 0;
    memcpy(&d->m_account, &req->m_account, 0x41);
    // 响应块 +0x16e 文本槽（ORIG 08100e5f `add $0x16e`）。
    strncpy(d->m_cmd.m_cancel.m_arg69, res->m_cmd.m_cancel.m_srcMsg16E, 0x15);
    strncpy(d->m_cmd.m_cancel.m_arg7E, req->m_cmd.m_output.m_price, 8);
    strncpy(d->m_cmd.m_cancel.m_arg92, req->m_cmd.m_output.m_arg7D, 0x14);
    strncpy(d->m_cmd.m_cancel.m_arg86, req->m_cmd.m_output.m_pay, 0xc);
    if (&req->m_cmd.m_output.m_desc91[0] != 0)   // ORIG 081006e3 `add $0x91,%eax; test %eax,%eax`：恒真遗留
    {
        strncpy(d->m_cmd.m_cancel.m_argA6, req->m_cmd.m_output.m_desc91, 100);
    }
    if (&req->m_cmd.m_output.m_descF5[0] != 0)   // 同上（ORIG 08100715）
    {
        strncpy(d->m_cmd.m_cancel.m_arg10A, req->m_cmd.m_output.m_descF5, 100);
    }
    // [b28d-solo-W6_IPGDeps] ORIG 0x08100e0a 尾部：res 指针单次装载 + 3 dword
    // 连续拷贝（0x1dc/0x1e0/0x1e4 → 0x1f0/0x1f4/0x1f8）⇒ 结构整体赋值形。
    d->m_cmd.m_cancel.m_tran = res->m_cmd.m_output.m_tran;
    d->m_notifyFlag = 0;
    m_mgr->PutIPGRequest(d, true);
}
