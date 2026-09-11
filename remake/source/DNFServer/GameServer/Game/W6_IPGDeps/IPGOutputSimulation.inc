// ORIG T 0x0810034e (0x18f)。返回值恒为请求临时块地址（调用前已释放）——ORIG 如此。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 108=108，残差仅寄存器角色）。
// 修法：删 out/rv 具名局部，全字段走 `req->m_cmd.m_output.*` /
// `res->m_cmd.m_output.*` 成员链（ORIG 各点 add $0x69/0x71/0x7d/0x91/0xf5/0x173
// 重算，P0/P183）。残差=虚调用目标寄存器 ORIG %edx vs 4.4.7 %eax（N7）。
// 双后端读数同 Finalize：4.4.6 后端下 AE。P167 CLEAR（11/11）。
SIPGData* CIPGHelper::IPGOutputSimulation(CUser* user,
                                          const char* p2, const char* p3,
                                          const char* p4, const char* p5,
                                          const char* p6, void* callback)
{
    SIPGData* req = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    // [b28d-solo-W6_IPGDeps] ORIG 0x0810034e 无 out/rv 具名中转（req 槽单承载，
    // 各字段 add $0x69/0x71/0x7d/0x91/0xf5/0x173 重算）⇒ 全成员链（P0/P183）。
    strncpy(req->m_cmd.m_output.m_price, p2, 8);
    strncpy(req->m_cmd.m_output.m_pay, p3, 0xc);
    strncpy(req->m_cmd.m_output.m_arg7D, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(req->m_cmd.m_output.m_desc91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(req->m_cmd.m_output.m_descF5, p6, 100);
    }
    req->m_callback = callback;

    SIPGData* res = m_mgr->AllocIPGData();
    memcpy(&res->m_account, &req->m_account, req->m_recordLen);  // 长度取 req+0 记录长（ORIG mov (%eax)）
    memcpy(res->m_cmd.m_output.m_resultCode, "0", 2);            // DAT_08b41446
    res->m_recordLen = req->m_recordLen;
    res->m_type = req->m_type;

    // ORIG: vptr+0x34 槽调用（IIPGHelperInterface slot11）= IPGResult(req,res)。
    this->IPGResult(req, res);

    m_mgr->FreeIPGData(req);
    m_mgr->FreeIPGData(res);
    return req;
}
