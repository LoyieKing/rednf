// ----------------------------------------------------------------------------
// ORIG T 0x0812e6de（sendClearStage.md 全文，const 方法）：ridableId=current
 // 数据对象 +0x18 u16 符号扩展；包体 _NotiAdvanceAltarClearStage(0x1c packed)
 // {stageType, clearRound, ridable, clearFlag, stage{idx,diff}, star}；
 // put_header(0,0x20f)+put_binary(0x1c)；rewards：put_int(size)+逐元素 0xc；
 // buyItems 同 0xc；rewardMap：put_int(size)+逐 value(_Reward 5B)；finalize→Send。
// ----------------------------------------------------------------------------
void ProcStage::sendClearStage(const std::vector<_RewardItem>& rewards,
                               const std::vector<_BuyItem>& buyItems,
                               const std::map<int, _Reward>& rewardMap,
                               int starCount, bool clearFlag) const
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }

    short ridableId = 0;
    AdvanceAltarData* current = user->m_advanceAltar.getCurrentAdvanceAltarData();
    if (current != 0)
    {
        // AdvanceAltarData(=_RidableBase)+0x18 = m_maxClearRound（short；objdump 0x0812e728 `movzwl 0x18(%eax)`）。
        ridableId = current->m_maxClearRound;
    }

    NOTIPacketStruct::_NotiAdvanceAltarClearStage body;
    body.m_stageType = getStageType();
    body.m_clearSurvivalRoundNo = getClearSurvivalRoundNo();
    body.m_ridableId = ridableId;
    body.m_clearFlag = clearFlag;
    // b28d-solo-ProcStage：ORIG 0x812e76b/0x812e776 逐字段直写 packed 子对象
    //   （`call getStageIndex; mov %eax,-0x3f` + `mov 0x68(%eax),%eax; mov %eax,-0x3b`）
    //   ⇒ 不得用 int[2]+memcpy（多一条 memcpy call，P167 EXTRA）。
    body.m_stage.m_field0 = getStageIndex();
    body.m_stage.m_field4 = static_cast<StageField4_t>(m_difficulty);
    body.m_starCount = starCount;

    PacketGuard guard;
    guard.put_header(0, 0x20f);
    guard.put_binary(reinterpret_cast<char*>(&body), 0x1c);
    // b28d-solo-ProcStage：ORIG 0x812e7c8 先 size() 缓存到 -0xc，put_int 复用缓存，
    //   随后 `cmpl $0,-0xc; jle <跳过循环>` —— 原码是 `int n=rewards.size();
    //   put_int(n); if (n > 0) { for(...) }`（buyItems/rewardMap 两段无此守卫）。
    const int rewardCount = static_cast<int>(rewards.size());
    guard.put_int(rewardCount);
    if (rewardCount > 0)
    {
        for (std::vector<_RewardItem>::const_iterator it = rewards.begin();
             it != rewards.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
        {
            // ORIG 0x812e820 调用 put_binary(char*,int)（Pc 非 const 重载；P167 差集：
            //   ORIG `_ZN18InterfacePacketBuf10put_binaryEPci` vs 旧 `EPKci`）⇒ C cast 去 const。
            guard.put_binary((char*)&*it, 0xc);
        }
    }
    guard.put_int(static_cast<int>(buyItems.size()));
    for (std::vector<_BuyItem>::const_iterator it = buyItems.begin();
         it != buyItems.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
    {
        guard.put_binary((char*)&*it, 0xc);
    }
    guard.put_int(static_cast<int>(rewardMap.size()));
    for (std::map<int, _Reward>::const_iterator it = rewardMap.begin();
         it != rewardMap.end(); it++)   // P28：ORIG 增量段 ppEi（后置）
    {
        // ORIG 0x812e944 `call operator->; add $0x4,%eax` ⇒ &it->second（Pc 版）。
        guard.put_binary((char*)&it->second, 5);
    }
    guard.finalize(true);
    user->Send(guard);
}