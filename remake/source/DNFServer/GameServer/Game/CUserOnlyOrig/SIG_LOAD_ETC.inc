SIG_LOAD_ETC::SIG_LOAD_ETC()
{
    new (reinterpret_cast<char*>(this) + 0x2308) SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO();  // 0x848f8c8
    new (reinterpret_cast<char*>(this) + 0x24a0) SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO();  // 0x848f8c8
    new (reinterpret_cast<char*>(this) + 0x29fd) SIG_LOAD_PREMIUM_LETHE();                  // 0x848ff7c
    new (reinterpret_cast<char*>(this) + 0x2b97) SIG_LOAD_PREMIUM_LETHE();                  // 0x848ff7c
    for (int i = 0; i < 0x31; ++i)
        new (reinterpret_cast<char*>(this) + 0x3e70 + i * 0xc) stNPCRelationship_t();       // 0x848fa50
    new (reinterpret_cast<char*>(this) + 0x40ce) SIG_REDEEMINFO_LIST_TO_DB();               // 0x848f894
    new (reinterpret_cast<char*>(this) + 0x466a) stTitleBook();                             // 0x848f62a
    new (reinterpret_cast<char*>(this) + 0xb1a4) stAchievement_DBData();                    // 0x828d8d8
    new (reinterpret_cast<char*>(this) + 0xc9af) SIG_SAVE_PC_ROOM_PLAY_TIME();              // 0x848fb64
    // +0xc9bc 的 std::bitset<32> 是 SIG_LOAD_ETC 的具名成员，编译器已在成员
    // 初始化阶段构造（对应 ORIG 0x84920c7 的唯一一次 bitset ctor 调用点）；
    // 此处再 placement new 会产生第二个 ctor 调用点，ORIG 无 → 删除。
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
