void CharacAdvanceAltarManager::setNextStage(int stageIdx)
{
    // [b28d-26] ORIG 0x081331d4 每处 `mov 0x8(%ebp),%eax; mov (%eax),%eax`
    // 重读 this+0（P183 地址重物化，无 cur 具名局部）⇒ 直读成员。
    // [b28d-solo-advancealtar_Manager] ORIG +0x10 `je <nop;leave>` ⇒ 正向包裹
    // （P222/P257），失败路径归函数尾。
    if (m_currentAdvanceAltarData != 0)
    {
    if (stageIdx == 0)   // 开场：插入 (firstStageId, {firstStageId, -1}) 占位
    {
        // [b28d-solo-advancealtar_Manager] ORIG 先 _Stage::C1 再 G_CDataManager
        // （无 StageMapList& 具名槽，P0 表达式直用）。
        _Stage stage;
        stage.m_field0 = G_CDataManager()->m_stageMapList.getFirstStageIndex();
        stage.m_field4 = static_cast<StageField4_t>(-1);   // b28d：enum 化（-1 哨兵）
        m_currentAdvanceAltarData->m_stages.insert(std::make_pair(stage.m_field0, stage));
        return;
    }
    bool chainFlags = false;
    const int nextStage =
        G_CDataManager()->m_stageMapList.getNextStage(stageIdx, chainFlags);
    if (nextStage == 0)
    {
        return;
    }
    // ORIG 有 iterator→const_iterator 转换构造 ×2 + operator==（P96/P270）
    // ⇒ 具名 const_iterator + 正极性 `== end`。
    std::map<int, _Stage>::const_iterator it =
        m_currentAdvanceAltarData->m_stages.find(nextStage);
    if (it == m_currentAdvanceAltarData->m_stages.end())
    {
        m_currentAdvanceAltarData->m_field20 = 1;   // ORIG *(char*)(cur+0x20)=1（语义【推断】）
        _Stage stage;
        stage.m_field0 = nextStage;
        stage.m_field4 = static_cast<StageField4_t>(-1);   // b28d：enum 化（-1 哨兵）
        m_currentAdvanceAltarData->m_stages.insert(std::make_pair(stage.m_field0, stage));
        m_currentAdvanceAltarData->m_update = 1;
        if (chainFlags)
        {
            setNextStage(nextStage);
        }
    }
    }
