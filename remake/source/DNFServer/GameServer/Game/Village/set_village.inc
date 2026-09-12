// [b28d-62] N89/P50：ORIG 用 `Area::~Area` + `operator delete[]`（数组删除），
//   我方 placement new/delete 形态 + 迭代器 cv 差。ORIG 0x086c33b8
bool Village::set_village(TownScript& script)
{
    // [g15 P348 试形 2026-09-11] ORIG 无 `movb $0x0,槽` 预清（我方帧头多 1 条）⇒ 去初始化式
    bool result;

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

    // [R20-03 2026-09-13] §26.3：ORIG 该条件为**内联短路**（0x086c34xx 段
    // `mov $1/%eax … test %al,%al; je`，无具名 bool 槽往返）；我方旧写
    // `bool bSchool = …; if (bSchool)` 多 `mov %al,-0x4e(%ebp); cmpb $0` 两条 ⇒ 内联。
    if (G_GameWorld()->IsSchoolPvPChannel() != 0 &&
        script.m_field0 == 2)
    {
        // [N3 2026-09-11 负结论] ORIG 该分支无 count/total 具名槽（`add $0x258,%eax;
        //   mov %eax,%ebx; imul $0x26,%ebx,%eax`，total 以 %ebx 跨 operator new[]
        //   保活 —— 4.1 前端对重复右值做 CSE）。实测删 count/total 改右值内联后
        //   our 526→523（ORIG 529，|gap| 3→6 劣化：4.4 -O0 无该 CSE，new[] 尺寸与
        //   raw[0] 两处各自重算）⇒ 按闸门回滚，保留具名局部。
        int count = m_areaCount;
        int total = count + 600;
        // [R18-03 2026-09-13] ④ 修复：ORIG 调用点多重集 = `_Znaj`×3（数组 new）+
        //   `Area::~Area`×3 + `_ZdaPv`×3（EH cleanup 链），**无** `_ZnwjPv`/`_ZdlPvS_`
        //   （ORIG 0x086c33b8 段实证；nm/p167 双侧核对）。旧手写 cookie + placement-new
        //   循环缺元素析构与 `operator delete[]`，元素构造中抛异常时 cleanup 语义分叉
        //   （已构造元素不析构、分配内存不释放）⇒ 恢复 ORIG 真形 `new Area[n]`。
        //   cookie 由编译器写（ORIG `mov %ebx,(%eax); lea 0x4(%eax)` 即该形）。
        m_pAreas = new Area[total];
        g_schoolMgr.SetSchoolArea(m_areaCount, m_areaCount + 600);
    }
    else if (script.m_field0 == 8)
    {
        int count = m_areaCount;
        int total = count + 0x960;
        // [R18-03 2026-09-13] 同上（④ 修复：恢复数组 new）。
        m_pAreas = new Area[total];
        // [b28d-sweep] 对象语义（ORIG 0x86c3685 `movl $0x943e0e0,(%esp)`，符号 B 0x58）
        g_guildAgitMgr.SetGuildAgitAreaDomain(m_areaCount, total);
    }
    else
    {
        int count = m_areaCount;
        // [R18-03 2026-09-13] 同上（④ 修复：恢复数组 new）。
        m_pAreas = new Area[count];
    }

    m_field2c = script.m_3c;
    m_questIdx = script.m_40;

    TownAreaScript temp;
    // [R20-03 2026-09-13] §26.3/P348：ORIG 无该槽预清（bSuccess 在 4.1 侧为
    // 寄存器值 `mov $0/%ebx`，0x086c385a/0x086c3a48；全函数读前必写）⇒ 去 `= false`。
    bool bSuccess;

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
            // [g15 P354 试形 2026-09-11] `!bool` 物化 xor（多 2 条）⇒ 改反向 if-else（空 then）消 xor
            if (bSuccess)
            {
            }
            else
            {
                goto cleanup;
            }
        }

    }
    result = true;

cleanup:
    return result;
}