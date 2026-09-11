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