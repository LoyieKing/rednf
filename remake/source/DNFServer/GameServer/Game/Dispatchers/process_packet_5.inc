int Dispatcher_UpgradeGage::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                           CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814031e] pReq→-0x14 / pRes→-0x10 / stage→-0xc；check_error
    // early-return；stage!=0 块内 4 组 getCharacData+方法（ORIG 不缓存 cd）。
    CMDPacketStruct::_ReqAdvanceAltarUpgradeGage* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarUpgradeGage*>(&req);
    CMDPacketStruct::_ResAdvanceAltarUpgradeGage* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarUpgradeGage*>(&resp);
    (void)pReq;
    // b28c-L5：清 2 处 `register asm` 存量绑定（P0 全域禁绑 / N96）。ORIG 0x0814031e
    //   `call check_error; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,(%eax)`
    //   = RHS 先 + eax→edx 中转，按 P211 双向开关由 **字段与 call 返回值符号性异型**
    //   触发 ⇒ m_nErrorCode 已换 unsigned int（CMDPacketStruct.h），自然形即复现。
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262
    if (stage != 0)
    {
        // b28c-L5：删 4 个 TU 内匿名 namespace 发明转发（P154 仅NEW 发明辅助），
        //   改为 ORIG 真实调用目标：StageControl::getCharacData T 0x081348a0 +
        //   CharacData::{getGageLevel W 0x08142094, upgreadGage T 0x0812d176,
        //   getGage W 0x08142088}；ORIG 每处重复取 cd（不缓存）⇒ 保持重复调用。
        pRes->m_nGageLevelBefore = stage->getCharacData()->getGageLevel();
        pRes->m_nErrorCode = stage->getCharacData()->upgreadGage();
        pRes->m_nGageLevelAfter = stage->getCharacData()->getGageLevel();
        pRes->m_nGage = stage->getCharacData()->getGage();
    }
    return 0;
