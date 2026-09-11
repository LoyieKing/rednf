void CDataManager::destroy()
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [b28d-solo-CDataManager AE] 本波 TU 口径已闭合（不在 DIFF/NEAR 列表）。
    // N7 封顶：delete 链中唯一虚析构调用（+0x86f4 vptr+4 间接 call）两侧
    // vptr/obj 寄存器互换（ORIG edx/eax vs 4.4 eax/edx，N7 族）；其余 307 条
    // 归一后逐条相等（fn_diff_tu 实证）。语义等价封顶。
    if (m_disJoint)
    {
        delete m_disJoint;
        m_disJoint = 0;
    }
    if (m_characList)
    {
        delete m_characList;
        m_characList = 0;
    }
    if (m_questList)
    {
        delete m_questList;
        m_questList = 0;
    }
    if (m_itemList)
    {
        delete m_itemList;
        m_itemList = 0;
    }
    if (m_skillList)
    {
        delete m_skillList;
        m_skillList = 0;
    }
    if (m_monsterManager)
    {
        delete m_monsterManager;
        m_monsterManager = 0;
    }
    if (m_mapList)
    {
        delete m_mapList;
        m_mapList = 0;
    }
    if (m_dungeonList)
    {
        delete m_dungeonList;
        m_dungeonList = 0;
    }
    if (m_itemUpgrade)
    {
        delete m_itemUpgrade;
        m_itemUpgrade = 0;
    }
    if (m_itemUpgradeSeparate)
    {
        delete m_itemUpgradeSeparate;
        m_itemUpgradeSeparate = 0;
    }
    if (m_emoticonList)
    {
        delete m_emoticonList;
        m_emoticonList = 0;
    }
    if (m_itemGeneratorMgr)
    {
        delete m_itemGeneratorMgr;
        m_itemGeneratorMgr = 0;
    }
    if (m_invalidChar.m_slangFilter)
    {
        // ORIG destroy：(**(code**)(vptr+4))(p) —— 经 vtable 第 2 槽调 SlangFilter
        // 虚删除析构 D0（权威类 vtable 槽1 即 deleting dtor），等价 delete。
        delete m_invalidChar.m_slangFilter;
        m_invalidChar.m_slangFilter = 0;
    }
    if (m_aiCharacterList)
    {
        delete m_aiCharacterList;
        m_aiCharacterList = 0;
    }
    if (m_autoMarket)
    {
        delete m_autoMarket;
        m_autoMarket = 0;
    }
    if (m_npcScriptList)
    {
        delete m_npcScriptList;
        m_npcScriptList = 0;
    }
    if (m_npcDynamicInfo)
    {
        delete m_npcDynamicInfo;
        m_npcDynamicInfo = 0;
    }
    if (m_npcRelation.m_itemLimitEditionMgr)
    {
        delete m_npcRelation.m_itemLimitEditionMgr;
        m_npcRelation.m_itemLimitEditionMgr = 0;
    }
    if (m_altarConfig.m_strikerScript)
    {
        delete m_altarConfig.m_strikerScript;
        m_altarConfig.m_strikerScript = 0;
    }
    if (m_altarConfig.m_characManageScript)
    {
        delete m_altarConfig.m_characManageScript;
        m_altarConfig.m_characManageScript = 0;
    }
    if (m_choiceItem.m_blueMarbleScriptMgr)
    {
        // ORIG destroy：BlueMarbleScriptManager::~BlueMarbleScriptManager(p) +
        // operator delete(p)（destroy.md 0x8357a66 尾段）→ 等价 delete。
        // 权威类（PvPTypes.h）无用户析构，隐式析构 + operator delete 由编译器生成。
        delete m_choiceItem.m_blueMarbleScriptMgr;
        m_choiceItem.m_blueMarbleScriptMgr = 0;
    }
}