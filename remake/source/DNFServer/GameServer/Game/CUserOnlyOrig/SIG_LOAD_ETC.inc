// ---- SIG_LOAD_ETC ctor（ORIG W 0x08491f86，size 0x269=617）----
// [R18arr 2026-09-13 重写] 原码（ORIG 0x08491f86）构造序列 = 对每个内嵌子对象发
//   `mov 0x8(%ebp),%eax; add $OFF,%eax; mov %eax,(%esp); call T::C1Ev`（29 call，
//   无任何 operator new）⇒ 原码是**真实具名成员**，全部由编译器在成员构造阶段自动
//   发射。本次把 [0x10 .. 0xc9b7] 段（含 +0x2308/+0x24a0/+0x29fd/+0x2b97/+0x3e70/
//   +0x40ce/+0x466a/+0xb1a4/+0xc9af 九个子对象区）改为 SigTypes_C.h 内的真实成员，
//   对应手写 placement new 全部删除（消除 9×`_ZnwjPv` + 相应 `_ZdlPvS_` EH 岛）。
// 残余（仍为手写 placement new）：+0xc9c0.. 段 11 处，其类型定义位于 SigTypes_D/E/B
//   分片且其中 CharacterOption / seriaRoom_AniDeco 定义在 CUserCharacInfo.h（含
//   SigTypes 反向依赖）⇒ 需分片结构变更方可成员化，本轮登记为后续工单。
//   该段在原码中本就连续位于成员段之后，故成员阶段 + ctor 体的相对构造序与 ORIG
//   一致（0x10 → … → 0xc9b7 → 0xc9c0 → … → 0x14b5c）。
SIG_LOAD_ETC::SIG_LOAD_ETC()
{
    // +0xc9bc 的 std::bitset<32> 是 SIG_LOAD_ETC 的具名成员，编译器已在成员
    // 初始化阶段构造（对应 ORIG 0x84920c7 的唯一一次 bitset ctor 调用点）。
    new (reinterpret_cast<char*>(this) + 0xc9c0) SIG_LOAD_SKILL();                          // 0x8491ee0
    new (reinterpret_cast<char*>(this) + 0xcdd5) SIG_REQUEST_PVP_MISSION_LIST();            // 0x848f8d4
    new (reinterpret_cast<char*>(this) + 0xd5a7) SIG_QUEST_SHOP_DATA();                     // 0x848f92e
    new (reinterpret_cast<char*>(this) + 0xd6fc) SIG_ITEM_MAKING_SKILL();                   // 0x848fa82
    new (reinterpret_cast<char*>(this) + 0xd718) SIG_REQUEST_TOD_USER_STATE();              // 0x8491f68
    new (reinterpret_cast<char*>(this) + 0xd73c) SIG_ITEM_DICTIONARY_FLAG();                // 0x848fab2
    new (reinterpret_cast<char*>(this) + 0xeac8) CharacterOption();                         // 0x822abec
    new (reinterpret_cast<char*>(this) + 0xecc8) SIG_BOOSTER_GAGE_DATA();                   // 0x848fb4c
    new (reinterpret_cast<char*>(this) + 0xecd0) SIG_BEST_CLEAR_TIME();                     // 0x848fb02
    new (reinterpret_cast<char*>(this) + 0x14aaa) SIG_COMBO_SKILL();                        // 0x848fbbe
    new (reinterpret_cast<char*>(this) + 0x14b5c) seriaRoom_AniDeco();                      // 0x822ee18
}
