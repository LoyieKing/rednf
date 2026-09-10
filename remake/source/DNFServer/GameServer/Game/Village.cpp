// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Village/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r Village（G2-5 村庄对象，sizeof 0x34）
// 逐函数对照 docs/class_func_reports/Village.md 与 ORIG 反汇编还原。
// 布局（ORIG ctor 086c32e0 / set_village 086c33b8 推导）：
//   +0x00 int    m_villageId      （set_village 写入 town script +0）
//   +0x04 pad（4）
//   +0x08 map<ushort,CUser*> m_users（0x18 字节，ctor 调 _ZNSt3map...C1Ev）
//   +0x20 bool   m_bEnabled
//   +0x24 int    m_areaCount
//   +0x28 Area*  m_pAreas         （new[] 数组，前 4 字节存元素个数）
//   +0x2c int    m_field2c        （set_village 写入 town script +0x3c）
//   +0x30 int    m_questIdx        （set_village 写入 town script +0x40；CheckMoveTown 读作 quest idx）
// 跨类调用一律 include 真实头（CDataManager.h / GameWorld.h / GameTypes.h /
// PacketGuard.h / TownAreaScript.h），std::map/vector 使用真实容器，
// 不再使用 extern asm 桥。
// ============================================================================

#include "GameTypes.h"        // cMyTrace
#include "CDataManager.h"     // G_CDataManager / CDataManager::get_dungeon
#include "CDungeon.h"         // CDungeon::m_vec6d4（set_village +0x6d4 具名化）
#include "PacketGuard.h"      // PacketGuard
#include "TownAreaScript.h"   // TownAreaScript
#include "CSchoolMgr.h"       // CSchoolMgr::SetSchoolArea / g_schoolMgr
#include "CGuildAgitManager.h" // CGuildAgitManager::SetGuildAgitAreaDomain / g_guildAgitMgr
#include "GameWorld.h"        // GameWorld::IsSchoolPvPChannel / G_GameWorld
#include "Village.h"          // Village（本 TU 实现）
#include "Area.h"             // Area（0x98 权威头）
#include "STQuestScript.h"    // TownScript（0x44 权威头）
#include "CMap.h"             // STMapScript（0x37c 权威头）
#include "CSchoolMgr.h"       // CBelong 完整类型（本 TU 承载其 C1 定义）

#include <map>
#include <new>
#include <string>
#include <vector>

// ImportMapScript（ORIG 0x089dcf54，自由函数；定义于 STMapScript.cpp，无权威头）
#include "Village/ImportMapScript.inc"

#include "Village/Village.inc"
{
    // m_users（map）由编译器隐式构造（ORIG 086c32e0：map C1 于 +0x08）
    m_pAreas = 0;
}

#include "Village/dtor_Village.inc"
{
    destroy();
    // m_users（map）由编译器隐式析构（ORIG：destroy() 后 map D1）
}

// [b28d-62] N18b：cmp 操作数序（写 `m_pAreas == pEnd` 亦不可翻）+ N112 对齐
//   nop；39=39，p167 CLEAR。ORIG 0x086c3348
#include "Village/destroy.inc"
{
    if (m_pAreas)
    {
        if (m_pAreas)
        {
            // §4.3 具名化：Area 步长由真实尺寸给出（sizeof(Area)==0x98 探针
            // 实证，Area.h 权威布局），不再写字面量 0x98。
            // [b28d-62] 去 count 具名局部（P0/P183）：ORIG 0x086c3366 直接
            //   `mov p; mov p; sub $4; mov (%eax),%eax; imul $0x98; lea (%edx,%eax,1),%ebx`
            //   —— 只有 pEnd 一个 callee-saved 缓存，count 是内联右值。
            register Area* pEnd = m_pAreas + ((int*)m_pAreas)[-1];
            // [b28d-62] ORIG 0x086c337d 布局为「cond 在头 + body 尾 jmp 回 cond」，
            //   4.4 对 while 旋转成前置 jmp ⇒ 改 for(;;)+break 复现（P283 反向）。
            for (;;)
            {
                if (m_pAreas == pEnd)
                    break;
                pEnd = pEnd - 1;   // §4.3（同上，负向遍历）
                pEnd->~Area();
            }
            ::operator delete[]((char*)m_pAreas - 4);
        }
        m_pAreas = 0;
    }
}

// [b28d-62] N89/P50：ORIG 用 `Area::~Area` + `operator delete[]`（数组删除），
//   我方 placement new/delete 形态 + 迭代器 cv 差。ORIG 0x086c33b8
#include "Village/set_village.inc"
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
        void* dungeon = G_CDataManager()->get_dungeon(script.m_18);
        // §4.3 具名化：get_dungeon 的 ORIG 实体是 find_dungeon 转发
        // （CDataManager.cpp:300 返 CDungeon*），+0x6d4 具名成员 = CDungeon::m_vec6d4
        // （CDungeon.h:749，std::vector<int>，public 区）。
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

#include "Village/getArea.inc"
{
    if (area < 0 || area >= m_areaCount)
    {
        // [b28c-35] ORIG 0x086c3bba：m_iAreaCount 在 cMyTrace::C1 调用**之前**读入
        // %ebx 并跨 ctor 保活（push %ebx; sub $0x24），具名对象形 4.4 会改在 ctor 后
        // 重读（无 push ebx，帧 0x28）。全表达式临时链（P227/P87）复现 ORIG 形。
        cMyTrace(__PRETTY_FUNCTION__, 0x356, 5)(
            "area(%d) < 0 || area >= m_iAreaCount(%d)",
            area, m_areaCount);
        return 0;
    }
    return &m_pAreas[area];
}

#include "Village/get_user_id_list.inc"
    int area,
    std::vector<unsigned short, std::allocator<unsigned short> >& list)
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0x36a, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].get_user_id_list(list);
    }
}

#include "Village/send_to_area.inc"
{
    if (area < 0 || area >= m_areaCount)
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0x360, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        m_pAreas[area].send_to_all(pkt);
    }
}

#include "Village/GetUserCount.inc"
{
    if (area < 0 || area >= m_areaCount)
        return 0;
    return m_pAreas[area].GetUserCount();
}

// CBelong::CBelong 定义置于本 TU（与消费调用者分离）：4.4 unit-at-a-time
// 对 TU 内可见 callee 塌缩 outgoing 帧（CSchoolMemberC1 sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 p6/p7 实证同族）。
#include "Village/CBelong.inc"
{
}
