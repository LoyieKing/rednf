// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x08140b3c orig/ours=38/36 ndiff-blocks=2
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,%edx | mov    -0xc(%ebp),%eax | mov    %edx,0x4(%eax)
int Dispatcher_Pause::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                     CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x08140b3c]：pReq→-0x10、pRes→-0xc 局部槽；resp+4 = check_error；
    // !=0 提前 return 0；==0 时 resp+8 = req+0xd（movzbl 宽度）并 Manager::pause。
    CMDPacketStruct::_ReqAdvanceAltarPause* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarPause*>(&req);
    CMDPacketStruct::_ResAdvanceAltarPause* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarPause*>(&resp);
    // ORIG 0x08140b3c：value 先落 %edx，地址槽后载 %eax（4.4 默认反向，register asm 钉牢）。
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    pRes->m_bPause = pReq->m_bPause;
    Manager::pause(user, pReq->m_bPause);                  // ORIG T 0x0813105c
    return 0;
}