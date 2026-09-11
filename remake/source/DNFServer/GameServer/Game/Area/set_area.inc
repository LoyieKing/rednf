// ---------------------------------------------------------------------------
// set_area：由 TownAreaScript/STMapScript 初始化区域（NPC/传送/怪物注册）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 38=38；313=313）。修法：pair 默认构造+逐字段
// 赋值、it/itEnd 具名提升、`(*it).m_x` 与 `it->m_npcId` 混用（deEv×2+ptEv×4）、
// G_CDataManager()->m_npcScriptList 内联（去 dataMgr/npcList 局部）、`(*it).m_vill` 具名
// 指针、m_bExistItemShopNPC bool 化 + 裸 `if(!field)`（P292/P248 档②）。本轮新增：
// ①**Zone 临时内联进 register_object 实参** ⇒ 4.4 把 s_timeGate_ 在 Zone ctor 前升格
//   %ebx 跨调用保活（P52 callee-saved 独占形，ORIG 086c1f16/1b 逐条同形）；
// ②**m_areaType int→unsigned int 异型化** ⇒ 复现 ORIG 086c1d44/47/4c 的
//   `mov 0x8(%eax),%eax; mov %eax,%edx; mov %edx,0x68` store 中转（P211/P274）。
void Area::set_area(int vill, TownAreaScript& script, STMapScript& mapScript)
{
    // [b28d-solo-Area] 原 N82 封顶判词作废：Zone 临时内联 + m_areaType unsigned 化
    // 后帧 0x8c 与槽位集合全等（见函数头判词）。
    m_npcIdList.clear();
    m_villageIndex = vill;
    m_areaIndex = script.m_areaIndex;
    m_areaType = script.m_areaType;
    m_field6c = script.m_field24;

    if (m_areaType == 2)
        createLotteryInfo(&m_lottery, &script.m_field18);

    if (m_areaType == 4)
        m_worldMapIndex = script.m_field18[0];
    else
        m_worldMapIndex = -1;

    if (!mapScript.m_assignNpcs.empty())
    {
        // [b28d-solo-Area] ORIG 086c1dcd 顺序 = pair C1Ev(-0x40) → itEnd=end()(-0x44)
        // → it=begin()(-0x48)，循环体只赋值不构造（pos/itEnd/it 均为循环外具名）。
        std::pair<int, int> pos;
        std::vector<AssignNpc>::iterator itEnd = mapScript.m_assignNpcs.end();
        std::vector<AssignNpc>::iterator it = mapScript.m_assignNpcs.begin();
        for (; it != itEnd; ++it)
        {
            // 坐标两处 `(*it).`（deEv，086d34da×2），npcId 四处独立 `it->`（ptEv）。
            pos.first = (*it).m_x;
            pos.second = (*it).m_y;
            m_npcPositions.push_back(pos);
            m_npcIdList.push_back(it->m_npcId);

            // [b28d-solo-Area] ORIG 086c1e64 `movzbl 0x94; xor $1; test; je`
            // ⇒ 字段 bool + 裸 `if(!field)`（P292/P248 档②）；赋值直存 %al。
            // 086c1e72.. 的 G_CDataManager()->m_npcScriptList 为内联表达式
            // （无 dataMgr/npcList 具名局部）。
            if (!m_bExistItemShopNPC)
            {
                CNPCScript* npcScript = (CNPCScript*)
                    G_CDataManager()->m_npcScriptList->find(
                        (unsigned int)it->m_npcId);
                if (npcScript != 0)
                {
                    m_bExistItemShopNPC =
                        npcScript->isExistRole(ENUM_NPC_ROLE_0);
                }
            }

            if (it->m_npcId == 0x7f)
            {
                GlobalData::s_timeGate_->SetTimeGate(vill,
                                          m_areaIndex, it->m_npcId,
                                          0x118a, 0x11bc);
                GlobalData::s_villageObjectMgr->register_object(
                    village_object::Zone((unsigned char)vill,
                                         (unsigned char)m_areaIndex),
                    (village_object::IObject*)GlobalData::s_timeGate_);
            }
        }
    }

    m_mapAreas = mapScript.m_mapAreas;
    for (unsigned int i = 0; i < mapScript.m_townPaths.size(); ++i)
    {
        // [b28d-solo-Area] ORIG 086c1fa1/086c1fbc 两次独立 operator[]（无引用缓存）。
        m_moveAbleArea.push_back(std::make_pair(
            mapScript.m_townPaths[i].m_field1c,
            mapScript.m_townPaths[i].m_field20));
    }

    stAttackedMapInfo_t* attackedInfo = 0;
    // [b28d-solo-Area] ORIG 086c202e/086c208a 每次 begin/end 前重新调
    // G_CDataManager()（无 infoList 具名局部，P157 逐次重读）。
    for (std::vector<stAttackedMapInfo_t>::iterator it =
             G_CDataManager()->m_villageAttacked.m_mapList.begin();
         it != G_CDataManager()->m_villageAttacked.m_mapList.end(); ++it)
    {
        // [b28d-solo-Area] ORIG 086c204d 解引用一次存具名指针 -0x1c（Ghidra
        // local_1c），再两次读该指针字段 ⇒ 具名 `&*it` 缓存（P0 有槽往返）。
        stAttackedMapInfo_t* pInfo = &*it;
        if (pInfo->m_vill == m_villageIndex && pInfo->m_area == m_areaIndex)
            attackedInfo = pInfo;
    }

    if (!mapScript.m_attackedMonsters.empty() && attackedInfo != 0)
    {
        GlobalData::s_villageMonsterMgr->InsertVillageMonster(
            m_villageIndex, m_areaIndex,
            attackedInfo->m_field8, attackedInfo->m_fieldC,
            &mapScript.m_attackedMonsters, &m_mapAreas);
    }
}