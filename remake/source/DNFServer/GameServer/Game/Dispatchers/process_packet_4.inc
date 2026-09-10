int Dispatcher_SummonUnit::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                          CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081405b8] pReq→-0x14 / pRes→-0x10 / stage→-0xc；回显 2 条 →
    // check_error early-return；stage!=0 块内：expect(req+0x14) 与 getGageLevel 比较
    // （getCharacData 每处重复调用，ORIG 不缓存 cd）。
    CMDPacketStruct::_ReqAdvanceAltarSummonUnit* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarSummonUnit*>(&req);
    CMDPacketStruct::_ResAdvanceAltarSummonUnit* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarSummonUnit*>(&resp);
    pRes->m_bySlotType = pReq->m_bySlotType;               // slotType 回显（byte）
    pRes->m_nUnitId = pReq->m_nUnitId;                     // unitId 回显（int）
    // ORIG 0x081405b8：value→%edx 先，槽地址→%eax 后（技巧28 变体）。
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262
    if (stage != 0)
    {
        if (pReq->m_nGageLevel != stage->getCharacData()->getGageLevel())
        {                                                  // [objdump 0x8140650] err=3
            pRes->m_nErrorCode = 3;
            return 0;
        }
        // [b28d-solo-S206] N-P211（跨 TU 依赖）：ORIG 0x814060x 该 store 为 LHS-first
        //   （`mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)`，objdump 直读）⇒ ORIG 中
        //   summonSlotUnit 返回与 unsigned 字段同型；我方声明 int（advancealtar_ProcStage.h:149，
        //   定义在非本泳道 .cpp）⇒ 调用点 cast 实测无效（P211「源侧 cast 不可翻」复证）。
        pRes->m_nErrorCode = stage->getCharacData()->summonSlotUnit(
            pReq->m_bySlotType, pReq->m_nUnitId);
        pRes->m_nGage = stage->getCharacData()->getGage();
    }
    return 0;
