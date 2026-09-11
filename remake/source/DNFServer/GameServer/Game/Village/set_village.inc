// [b28d-62] N89/P50：ORIG 用 `Area::~Area` + `operator delete[]`（数组删除），
//   我方 placement new/delete 形态 + 迭代器 cv 差。ORIG 0x086c33b8
bool Village::set_village(TownScript& script)
{
    bool result = false;

    m_villageId = script.m_field0;
    if (script.m_18 == -1)
    {
        m_bEnabled = false;
    }
    else
    {
        m_bEnabled = true;
        // §4.3 具名化：get_dungeon 的 ORIG 实体是 find_dungeon 转发
        // （CDataManager.cpp:300 返 CDungeon*），+0x6d4 具名成员 = CDungeon::m_vec6d4
        // （CDungeon.h:749，std::vector<int>，public 区）。
        // [Q8 2026-09-11 旧N判词重扫·试形·回滚] P0/P183 删 dungeon / v 两个具名
        //   局部（ORIG 0x086c33f6 区段确无该两槽往返）**实测劣化 gap**：
        //   原 530 条 gap=−1 blocks=71 → 删 dungeon 528 gap=+1 blocks=70 →
        //   再删 v 526 gap=**+3** blocks=66。ORIG 529 的多余条数来自 4.1 前端
        //   对 `add $0x258,%eax; mov %eax,%ebx; imul $0x26,%ebx,%eax` 的寄存器
        //   CSE/保活（4.4 -O0 无），删局部只让 4.4 更精简、反而远离 ORIG 指令数
        //   （同 [N3] 判词「删 count/total ⇒ ours 526→523 劣化」同族）⇒ 按闸门
        //   回滚，保留具名局部形。
        void* dungeon = G_CDataManager()->get_dungeon(script.m_18);
        std::vector<int>* v = &static_cast<CDungeon*>(dungeon)->m_vec6d4;
        v->push_back(m_villageId);
    }

    m_areaCount = (int)script.m_areas.size();
    if (m_areaCount == 0)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__, 0x2d0,
            "town.townAreaScripts_.size() == 0");
        return false;
    }

    bool bSchool = G_GameWorld()->IsSchoolPvPChannel() != 0 &&
                   script.m_field0 == 2;
    if (bSchool)
    {
        // [N3 2026-09-11 负结论] ORIG 该分支无 count/total 具名槽（`add $0x258,%eax;
        //   mov %eax,%ebx; imul $0x26,%ebx,%eax`，total 以 %ebx 跨 operator new[]
        //   保活 —— 4.1 前端对重复右值做 CSE）。实测删 count/total 改右值内联后
        //   our 526→523（ORIG 529，|gap| 3→6 劣化：4.4 -O0 无该 CSE，new[] 尺寸与
        //   raw[0] 两处各自重算）⇒ 按闸门回滚，保留具名局部。
        int count = m_areaCount;
        int total = count + 600;
        // [P6S6 2026-09-11 负结论·回滚] p167 线索「ORIG 本件 MISSING `Area::~Area`
        //   x3 + `_ZdaPv` x3、EXTRA `_ZnwjPv` x3」提示 ORIG 用标准 `new Area[n]`，
        //   故本波实测把 placement-new 循环改为 `m_pAreas = new Area[total];`：
        //   读数**劣化** —— ours 526→**531**（ORIG 529），difflib 块 68→69，|gap| 3→2
        //   反号（+2）⇒ 4.4 的 new[] EH/cookie 框架比 ORIG 多 5 条，按闸门回滚。
        //   ORIG 那 3 组 `~Area`/`_ZdaPv` 调用点应属 4.1 对 new[] 的 EH 清理器形，
        //   4.4 走 `_ZnwjPv`+`_ZdlPvS_`（placement 族）⇒ 属跨版本发射差，非语义缺口。
        int* raw = (int*)::operator new[]((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3：步长由 Area 真实尺寸给出（sizeof==0x98 探针实证）
        }
        m_pAreas = (Area*)(raw + 1);
        g_schoolMgr.SetSchoolArea(m_areaCount, m_areaCount + 600);
    }
    else if (script.m_field0 == 8)
    {
        int count = m_areaCount;
        int total = count + 0x960;
        int* raw = (int*)::operator new[]((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3（同上）
        }
        m_pAreas = (Area*)(raw + 1);
        // [b28d-sweep] 对象语义（ORIG 0x86c3685 `movl $0x943e0e0,(%esp)`，符号 B 0x58）
        g_guildAgitMgr.SetGuildAgitAreaDomain(m_areaCount, total);
    }
    else
    {
        int count = m_areaCount;
        int* raw = (int*)::operator new[]((count * 0x26 + 1) * 4);
        raw[0] = count;
        Area* pArea = (Area*)(raw + 1);
        for (int i = count - 1; i != -1; --i)
        {
            ::new (pArea) Area();
            pArea = pArea + 1;   // §4.3（同上）
        }
        m_pAreas = (Area*)(raw + 1);
    }

    m_field2c = script.m_3c;
    m_questIdx = script.m_40;

    TownAreaScript temp;
    bool bSuccess = false;

    // [P6S6 2026-09-11] ORIG 循环头 = `_ZNKSt17_Rb_tree_iterator…ne`（!= 条件）
    //   + `ppEi`（后置 ++）⇒ 改为真 for 条件形（it != end(); it++）。
    //   实测：TU 四数 ae 55→**56**、missing_in_game 19→**18**（p167fix 的
    //   `…_Rb_tree_iterator…ne`/`ppEi` 两条 MISSING + `eq`/`ppEv` 两条 EXTRA
    //   全 CLEAR）；set_village 自身 gap −3→+1（ours 526→530，ORIG 529）。
    for (std::map<int, TownAreaScript>::iterator it = script.m_areas.begin();
         it != script.m_areas.end(); it++)
    {
        {
            TownAreaScript copy(it->second);
            temp = copy;
        }

        int areaIndex = temp.m_areaIndex;
        if (areaIndex < 0 || areaIndex >= m_areaCount)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x32e, 5);
            trace("temp_area.areaIndex_ %d", areaIndex);
        }
        else
        {
            STMapScript mapScript;
            const char* mapName = temp.m_mapName.c_str();
            if (ImportMapScript(&mapScript, mapName) == 1)
            {
                m_pAreas[areaIndex].set_area(m_villageId, temp, mapScript);
                if (G_GameWorld()->IsSchoolPvPChannel() != 0 &&
                    script.m_field0 == 2 && areaIndex == 4)
                {
                    for (int i = m_areaCount; i < m_areaCount + 600; ++i)
                    {
                        temp.m_areaIndex = i;
                        m_pAreas[i].set_area(m_villageId, temp, mapScript);
                    }
                    m_areaCount += 600;
                }
                if (script.m_field0 == 8 && areaIndex < 4)
                {
                    int nEnd = m_areaCount + 0x960;
                    int nStart = 0;
                    // [N3 2026-09-11] ORIG 0x086c3992..0x086c39ed 为**顺序 if-else 链**
                    //   （每段重新 `mov -0x98(%ebp),%eax` 载入 areaIndex：
                    //    0x086c3992 `test %eax,%eax; jne` → 0x086c39ab `cmp $0x1,%eax; jne`
                    //    → 0x086c39c4 `cmp $0x2,%eax; jne` → 0x086c39ed `cmp $0x3,%eax; jne`），
                    //   非 4.4 的 switch 二分决策树（旧形 `cmp $1/je + cmp $1/jg + test/je`）
                    //   ⇒ 改真 if-else 链复现（P320 反向：真 switch → 手写链）。
                    if (areaIndex == 0)
                        nStart = m_areaCount;
                    else if (areaIndex == 1)
                        nStart = m_areaCount + 1;
                    else if (areaIndex == 2)
                        nStart = m_areaCount + 2;
                    else if (areaIndex == 3)
                    {
                        nStart = m_areaCount + 3;
                        m_areaCount += 0x960;
                    }
                    for (int i = nStart; i < nEnd; i += 4)
                    {
                        temp.m_areaIndex = i;
                        m_pAreas[i].set_area(m_villageId, temp, mapScript);
                    }
                }
                bSuccess = true;
            }
            else
            {
                result = false;
                bSuccess = false;
            }
            if (!bSuccess)
                goto cleanup;
        }

    }
    result = true;

cleanup:
    return result;
}