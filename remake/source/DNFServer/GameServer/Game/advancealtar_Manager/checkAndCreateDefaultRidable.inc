void CharacAdvanceAltarManager::checkAndCreateDefaultRidable()
{
    // [b28d-solo-advancealtar_Manager] ORIG：while 形（前置 jmp cond + 尾部
    // `cmpl/setle/test/jne`，P25/P51）；m_data[i] 自然寻址（ORIG imul $0xd0）；
    // size()==0 短路链（P197④，禁 empty()）；无 data/allEmpty 具名局部（P0）。
    bool skipInit = false;
    AdvanceAltarData* target = 0;
    int i = 0;
    while (i <= 0)
    {
        if (m_data[i].m_ridableId == RidableId::RIDEABLE)
        {
            if (m_data[i].m_buyItems1.size() == 0 &&
                m_data[i].m_buyItems2.size() == 0 &&
                m_data[i].m_buyItems3.size() == 0)
            {
                target = &m_data[i];     // ORIG local_14 = this+i*0xd0+4
                break;
            }
            skipInit = true;             // 已有内容不重置
        }
        i++;
    }
    if (skipInit)
    {
        return;
    }
    if (target == 0)
    {
        target = &m_data[0];            // ORIG 回退 this+4
    }
    target->m_update = 1;
    target->m_ridableId = RidableId::RIDEABLE;
    setCurrentAdvanceAltarData(target->m_ridableId);   // ORIG 重读 (target)
    setDefaultSlot();
    setDefaultStage();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);
    target->m_ticket = G_CDataManager()->m_altarConfig.m_0;   // ORIG +0x6364 配置票数
