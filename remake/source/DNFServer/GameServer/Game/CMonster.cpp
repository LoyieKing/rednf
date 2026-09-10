// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMonster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r Game/ CMonster 实现。
#include <utility>
#include <vector>

#include "STMonsterScript.h"
#include "CMonster.h"


#include "CMonster/get_rand_int.inc"

#include "CMonster/_drop_item_index.inc"
                     const std::vector<std::pair<int, int> >& entries)
{
        // ORIG 0x08349672：先清零 accumulated，再构造 entry；循环内经 make_pair
    // 临时对走 pair::operator=(pair&&)（O0 生成 move op= 调用，非内联拷贝）。
        int accumulated = 0;
    std::pair<int, int> entry;
    for (std::vector<std::pair<int, int> >::const_iterator it = entries.begin();
         it != entries.end(); it++)
    {
                entry = std::pair<int, int>(*it);
        accumulated += entry.second;
        if (randomValue < accumulated)
            return entry.first;
    }
    return -2;
}

#include "CMonster/CMonster.inc"
{
    // ORIG 0x08379ea4：仅构造 vector 成员与 m_script，标量不初始化
    // （值由 set_monster 填充；m_index/m_specialItemWeight 等保持未定值）。
}

#include "CMonster/dtor_CMonster.inc"
{
}

#include "CMonster/get_index.inc"
{
    return m_index;
}

#include "CMonster/getMonsterScript.inc"
{
    return const_cast<STMonsterScript*>(&m_script);
}

#include "CMonster/set_monster.inc"
{
    // ORIG 0x0834f650：逐字段拷贝（script → this），非整体赋值：
    //   this+0x04 ← script+0x00(m_index)；this+0x08.. ← script+0x28 的 4 个
    //   specialItemLists；this+0x38 ← script+0x88；this+0x3c ← script+0x8c；
    //   this+0x58 ← script+0x3ec；this+0x5c ← script+0x3f0；
    //   this+0x1d24 ← script+0xc4c；this+0x54 ← script+0x3a8(byte)；
    //   this+0x48 ← script+0x58；最后 STMonsterScript::operator=(this+0x60, script)。
    m_index = script.m_index;
    for (int i = 0; i < 4; ++i)
        m_specialItemLists[i] = script.m_field028[i];
    m_specialItemWeight = script.m_field088;
    m_dropItems = script.m_field08c;
    m_independentDropCount = script.m_levelByDiff[0];
    m_catchItemCount = script.m_levelByDiff[1];
    m_weight = script.m_fieldc4c;
    m_deathTowerFlag = script.m_flag3a8;   // ORIG movzbl 0x3a8(%e) → %dl 直存 +0x54（bool 成员单指令 disp）
    m_deathTowerItems = script.m_field058;
    m_script = script;   // 隐式 op=（ORIG W 0x08377994 编译器合成版）
}

#include "CMonster/IsHellMonster.inc"
{
    return m_deathTowerFlag;
}

#include "CMonster/get_specialItem_list_size.inc"
{
    // ORIG 0x0834f746：cmpl $0x3; ja 跳 return 0，fall-through 为 size()。
    if (index <= 3)
        return static_cast<unsigned int>(m_specialItemLists[index].size());
    return 0;
}

#include "CMonster/deathTowerSpecifyItemDrop.inc"
{
    // ORIG 0x0834f84a：无 empty 检查，直接 get_rand_int(10000) 后转发。
    return _drop_item_index(get_rand_int(10000), m_deathTowerItems);
}

#include "CMonster/drop_item_index.inc"
{
    // ORIG 0x0834f776：m_dropItems(+0x3c) 非空 → get_rand_int(m_specialItemWeight@+0x38)
    // 对 m_dropItems；空 → specialItemLists[index](+0x8)。无 index 检查。
    if (!m_dropItems.empty())
        _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
    else
        _drop_item_index(get_rand_int(10000), m_specialItemLists[index]);
}

#include "CMonster/drop_item_index_1.inc"
{
    // ORIG 0x0834f7e4：同单参版，空分支直接用传入 randomValue；两分支均
    // tail-jmp 保留 eax = _drop_item_index 返回值（调用点取回判 != -2）。
    if (!m_dropItems.empty())
        return _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
    else
        return _drop_item_index(randomValue, m_specialItemLists[index]);
}
