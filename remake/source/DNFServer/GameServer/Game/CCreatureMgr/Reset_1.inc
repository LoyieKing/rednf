void CCreatureMgr::Reset()
{
    // [b28-BM7 判词] 残余 = N82/N89 复合：①ORIG 帧 0x48（迭代器槽 -0x1c/
    // -0x18 深排），4.4 紧凑 0x38 浅排（double framePad×2 实测反产 0x58/
    // -0x2c 更漂，P190 附注边界）；②begin/end/++/clear 调用 4.1 发射
    // out-of-line 模板实例（N89），4.4 内联。cMyTrace arg 已按 P118 改
    // __PRETTY_FUNCTION__（0x833a514 _ZZ 符号实证）。
    m_equippedCreature = 0;            // +0x18
    m_user = 0;                        // +0x1c
    m_tempExpireDelta = -1;            // +0x2c = -1
    if (m_field30.size() != 0)         // +0x30 map clear
        m_field30.clear();
    if (m_creatureItems.size() != 0)   // this+0x00 map<int,CCreatorItem*>
    {
        for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
             it != m_creatureItems.end(); it++)   // P167：ORIG 增量段为后置 ppEi
        {
            CCreatureItem* creatureItem = it->second;
            if (creatureItem != 0)
            {
                if (!GetInstanceCreatureItemFactory()->FreeCreatureItem(creatureItem))
                {
                    // P118/P139：ORIG 0x833a514 arg2 = `_ZZN13user_creature12
                    // CCreatureMgr5ResetEvE19__PRETTY_FUNCTION__` 局部静态。
                    LogManager::logFormat(1, "Creature.cpp",
                                          __PRETTY_FUNCTION__,
                                          0xb50, "FreeCreatureItem() error");
                }
            }
        }
        m_creatureItems.clear();
    }
    for (int i = 0; i < 3; ++i)        // CArtifact::Reset(this+(i+8)*4)
        m_artifacts[i].Reset();
    m_isMaxLevelGrowCreature = 1;      // +0x48 byte = 1
