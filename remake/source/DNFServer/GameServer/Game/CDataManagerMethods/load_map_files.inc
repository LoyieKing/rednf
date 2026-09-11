// ===================== load_map_files(vector<PVPMAP_DATA>&) =====================
// [b28d-solo-CDataManagerMethods] NEAR（orig=316 our=316 gap=0，P167 CLEAR 44/44）。
// 关键修法：dungeon/map 提升为函数级具名局部、it/stageIt 先声明后赋值
// （P50 复活）、两处后置 `it++`（ORIG 083416e4=ppEi）、pos 改 pair 整体赋值
// （单次 operator[]）。残差 = 全帧槽号平移 0x10（4.4 把 set_map 按值参数临时
// 放浅、具名局部随之浅移）⇒ N82/P112 槽分配族封顶。
// ORIG 0x0835fe02：遍历 g_stMapScriptFileList（map<int,string>）。
// 键==50000 仅 Clear；否则 ImportMapScript 导入（失败打印并返回 0）。
// 每个文件 new CMap + set_map(拷贝)。m_list4 为空时仅以 (15,15) 注册进
// m_mapList；否则逐 dungeon 下标（find_dungeon 失败 trace 后返回 0）
// 按 m_recoveryObjects 各坐标注册进 dungeon 与 m_mapList；m_dungeonStartArea.m_w>0 时额外 insertStart。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
// [l5f 2026-09-11 · §6.5/P118 串盲区] ORIG 定义签名形参写作 typedef
// PVPMAP_VECTOR（ORIG .rodata 串实证：`bool CDataManager::load_map_files(
// PVPMAP_VECTOR&)`，我方旧串为 std::vector<PVPMAP_DATA, std::allocator<...> >&）。
// typedef 与底层类型同一 ⇒ mangled/布局/sizeof 零变化，仅改 __PRETTY_FUNCTION__
// 展开串（AE 口径把 .rodata 归 <A> 故此前不可见）。声明侧 CDataManager.h:3675
// 拼写不同但类型同一，无需改动。
typedef std::vector<PVPMAP_DATA> PVPMAP_VECTOR;
bool CDataManager::load_map_files(PVPMAP_VECTOR& pvpMaps)
{
    const char* filename = 0;
    STMapScript script;
    CDungeon* dungeon = 0;
    CMap* map = 0;
    std::map<int, std::string>::iterator it;
    std::list<int>::iterator stageIt;
    it = g_stMapScriptFileList.begin();
    std::pair<int, int> pos;
    while (it != g_stMapScriptFileList.end())
    {
        filename = it->second.c_str();
        if (filename == 0)
        {
            break;
        }
        if (it->first == 50000)
        {
            script.Clear();
        }
        else if (!ImportMapScript(&script, filename))
        {
            printf("Error in script('%s')\n", filename);
            return 0;
        }

        // ORIG 同样不释放错误路径上的 CMap（忠实转写）。
        map = new CMap();
        map->set_map(script);

        if (script.m_list4.size() == 0)
        {
            m_mapList->insert_map(15, 15, map, pvpMaps);
            it++;
            continue;
        }

        stageIt = script.m_list4.begin();
        while (stageIt != script.m_list4.end())
        {
            int stageIdx = *stageIt;
            dungeon = m_dungeonList->find_dungeon(stageIdx);
            if (dungeon == 0)
            {
                // [l5f 2026-09-11 · P221 落地] 全表达式临时链：ORIG 该 cMyTrace
                // 临时占浅槽 -0x44（深区具名对象整体下沉 0x10）；两语句具名形令
                // 4.4 把它排到最深槽 -0x3dc 并顶起全部深区对象。
                // 实测：NEAR(317/317) → AE(317/317)，TU near 1→0 / ae 165→166。
                cMyTrace(__PRETTY_FUNCTION__, 0x254b, 5)
                    ("[Load Map] Not exist dungeon. [Index:%d] filename : [%s]",
                     stageIdx, filename);
                return 0;
            }

            if (script.m_recoveryObjects.size() == 0)
            {
                dungeon->InsertMap(15, 15, map);
                m_mapList->insert_map(15, 15, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(15, map);
                }
            }

            for (int i = 0; i < static_cast<int>(script.m_recoveryObjects.size()); ++i)
            {
                pos = script.m_recoveryObjects[i];
                dungeon->InsertMap(pos.first, pos.second, map);
                m_mapList->insert_map(pos.first, pos.second, map, pvpMaps);
                if (script.m_dungeonStartArea.m_w > 0)  /* +0x148 = m_dungeonStartArea.m_w（新 0x1c MapArea 吸收旧独立 int）*/
                {
                    dungeon->insertStart(pos.first, map);
                }
            }
            stageIt++;
        }
        it++;
    }
    return 1;
}