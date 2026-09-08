// ============================================================================
// BattleFieldLCB.cpp —— LinkClosureB.BattleItem 独占 TU。
// 忠实转写三个 CBattle_Field ORIG 符号（报告缺失者按 objdump 直转）：
//   1. kill_blood_monster(CUser*, int, unsigned short*)
//        T _ZN13CBattle_Field18kill_blood_monsterEP5CUseriPt @0x0830d58c (0x178)
//   2. DestroyPassiveObject(int)
//        T _ZN13CBattle_Field20DestroyPassiveObjectEi     @0x082ff3ec (0x99)
//   3. SetAssignNPCByPotion(bool)
//        W _ZN13CBattle_Field20SetAssignNPCByPotionEb     @0x085d19d6 (0x23，无
//          报告，objdump 兜底转写)
// ============================================================================

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h

// ---------------------------------------------------------------------------
// 1. kill_blood_monster  @0x0830d58c
//
// ORIG 控制流（报告 docs/class_func_reports/CBattle_Field/kill_blood_monster.md）：
//   - this+0x188（m_pDungeon）== NULL            → return 0；
//   - m_pDungeon+0x89f（m_bloodDungeonType）== 0 → return 0；
//   - cur = GetCurrentMapInfo()；对 uids[i]（count 个 unsigned short）在
//     cur->m_monsterMap（MapInfo+0x0c，std::map<int,map_monster>）中 find；
//   - 命中时拷贝 map_monster（ORIG 拷贝仅用于判断），拷贝体 +0x344
//     （m_bloodKillFlag）== 1 时 erase(it) 并置标志；
//   - 有擦除则 checkKilledSpawnBloodMonsters(m_monsterMap.size())；
//   - return 1（blood 分支）/2 个提前 return 0。
// param_1(user) 在 ORIG 中完全未被使用，保持形参以匹配 ABI。
// ---------------------------------------------------------------------------
bool CBattle_Field::kill_blood_monster(CUser* user, int count,
                                       unsigned short* uids)
{
    if (!m_pDungeon)                                   // this+0x188
        return false;
    if (!m_pDungeon->m_bloodDungeonType)               // +0x89f 字节非 0 才继续
        return false;

    // b28d-60：ORIG 先 `movb $0x0,-0x11` 再 call GetCurrentMapInfo ⇒ 原码
    // erased 先声明（4.1 先声明占更深槽 -0x11，cur 后占 -0x10）。
    bool erased = false;                               // 栈上 local_15
    MapInfo* cur = GetCurrentMapInfo();                // 0822cbcc
    // b28d-60：P183 去 `monsterMap` 引用缓存（ORIG 无该槽，每处重算
    // `mov -0x10(%ebp),%eax; lea 0xc(%eax),%edx`；原引用槽把 cur 挤到 -0x18）。
    for (int i = 0; i < count; ++i)                    // local_10 循环
    {
        const int uid = static_cast<int>(uids[i]);
        std::map<int, map_monster>::iterator it = cur->m_monsterMap.find(uid);
        // b28d-60：P220/P292 正极 continue 守卫——ORIG `call operator==; test;
        // jne <循环尾 nop 岛>` 与 `movzbl; xor $1; test; jne`（!flag）两处都是
        // 「条件成立即跳到循环尾」，原码为两条 continue 守卫（非 if(it!=end){}）。
        if (it == cur->m_monsterMap.end())
            continue;
        // ORIG: map_monster copy ctor 08151aea，随后测 +0x344 布尔取反
        map_monster mob(it->second);
        if (!mob.m_bloodKillFlag)
            continue;
        cur->m_monsterMap.erase(it);                   // map::erase 08311946
        erased = true;
    }
    if (erased)
        checkKilledSpawnBloodMonsters(
            static_cast<int>(cur->m_monsterMap.size()));  // size 后调 08304f8e
    return true;
}

// ---------------------------------------------------------------------------
// 2. DestroyPassiveObject  @0x082ff3ec
//
// ORIG 控制流（报告 DestroyPassiveObject.md）：
//   - this+0x108（m_pMap）== NULL → return 0；
//   - vec = m_pMap->m_passiveObjectList（CMap+0xa8）；empty() → return 0；
//   - 遍历元素 == idx → return 1；
//   - 否则 return 0。
// ---------------------------------------------------------------------------
bool CBattle_Field::DestroyPassiveObject(int idx)
{
    // b28d-60：补 ORIG 的 empty() 调用（原缺 = 语义缺口 N-88b）+ P222 正向包裹
    //（ORIG `test;je <尾 mov$0>`，非提前返回）+ P183 去 list 引用缓存（ORIG 每处
    // 重算 `mov 0x108(%eax),%eax; add $0xa8,%eax`，无 -0x10 槽）。
    if (m_pMap)                                        // this+0x108
    {
        if (m_pMap->m_passiveObjectList.empty())       // CMap+0xa8
            return false;
        for (std::size_t i = 0; i < m_pMap->m_passiveObjectList.size(); ++i)
        {
            if (m_pMap->m_passiveObjectList[i] == idx)
                return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// 3. SetAssignNPCByPotion  @0x085d19d6（W 弱符号；无独立报告，objdump 直转）
//
// objdump 全量（0x85d19d6..0x85d19f9）：
//   push %ebp / mov %esp,%ebp / sub $0x28,%esp
//   movzbl 0xc(%ebp),%edx           ; 取 bool 实参低字节
//   call GetCurrentMapInfo (0822cbcc)，this=0xc(%ebp)?? —— 见下注
//   mov %dl,0xa4(%eax)              ; 写 MapInfo.m_assignNPCByPotion (+0xa4)
//   leave / ret
// 即：GetCurrentMapInfo()->m_assignNPCByPotion = flag。
// ---------------------------------------------------------------------------
void CBattle_Field::SetAssignNPCByPotion(bool flag)
{
    // ORIG：无 cur 局部（mapInfo eax 直通），bool 形参直读形参槽。
    GetCurrentMapInfo()->m_assignNPCByPotion = flag;
}
