// ============================================================================
// DB_LoadEtc::LoadAchievement @0x08406422
// achievement zip → SIG_LOAD_ETC+0xb1a4（cap 0x1800），统计非零 0xc 条目计数入
// +0xc9a4，unix_timestamp(last_update_time) 入 +0xc9a8。
// ============================================================================
// [b29-Lane21] 已修：`unsigned int srcLen` ⇒ 实参装载序 srcLen 位置差消。
// 残差（N 封顶）：①N1 常量拆分（ORIG `mov sig,%ecx; …; add $0xb1a0,%eax; mov 0x4(%eax),%eax`
//   vs 4.4 `add $0xb1a4,%eax; mov (%eax),%eax`——元素内 +4 被折入基址常量）；
//   ②N7 dst 取址 add/lea + `mov 0xc(%ebp),%ecx` 装载序。
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量合并 + P75 实参装载序）：ORIG 0x08406422。
// 已修（本波）：count 循环改 12 字节元素视图 `AchievementEntry m_entries[0x200]`
//   （基址 0xb1a0 + 元素内 +4，替代 `m_expandDataF + i*12` 的 cast 形）。
// 残差（不可翻，两处，余者全等 + nop 豁免）：
//   ①uncompress_zip 实参装载序——ORIG 先第 1 参（`mov sig,%eax; add $0xb1a4,%eax`）后
//     第 4 参 srcLen；4.4 恒先第 4 参（P75 跨版本实参装载时刻；删冗余 cast 无效）。
//   ②循环基址——ORIG `lea (sig,i*12); add $0xb1a0,%eax; mov 0x4(%eax)`（基址 sig 原始、
//     disp4 保留）；4.4 恒把基址偏移与元素内偏移合并为 `add $0xb1a4` + `mov (%eax)`
//     （N1 常量折叠；offset-0/成员数组/short 元素三形实测均合并）。
//     [b28d-solo 复测] 补测第四形：二维具名数组视图 `int m_entries[0x200][3]` +
//     `m_entries[i][1]` ⇒ 4.4 仍折 `add $0xb1a4` + `mov (%eax)`（P311 于本偏移不适用），
//     127=127 条；四形均合并 ⇒ 封顶。
// 已修（前波）：exec 具名 ok、outLen 顺序、get_uint 直写 m_achievementLastUpdate（去局部）。
bool DB_LoadEtc::LoadAchievement(SIG_LOAD_ETC* sig)
{
    static const int compressed_buff_size = 0x1cd8;  // ORIG _ZZ…LoadAchievement…E20compressed_buff_size
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect achievement, unix_timestamp(last_update_time) from"
                  " charac_achievement where charac_no=%u", sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0x15 具名槽
    if (!ok)
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    // ORIG 0x084064cf：rep stos edx=0x736 **dword** 计数 = 0x1cd8 字节（P253 全清），
    // `={0}` 触发 4.1/4.4 的 rep stos 展开（P214③）。
    char blob[0x1cd8] = { 0 };
    if (!db->get_binary(0, blob, compressed_buff_size))
    {
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    unsigned int outLen = 0x1800;      // ORIG 0x08406531：srcLen 之后才初始化
    // [K2ADBLogin 2026-09-11] 负结论（同 LoadRedeemItem）：显式指针算术形实测
    //   仍产 `lea 0xb1a4(%eax),%edx`（ORIG 为 `add $0xb1a4,%eax`）⇒ N107 反向封顶。
    if (!uncompress_zip((char*)&sig->m_achievement, outLen, blob, srcLen))
    {
        return false;
    }

    int count = 0;
    // ORIG 0x0840658c：`mov sig,%ecx; i*12; lea (%ecx,%eax,1),%eax; add $0xb1a0,%eax;
    //   mov 0x4(%eax),%eax`——基址偏移 0xb1a0 非 12 倍数 ⇒ 4.4 保留独立 add（不折 disp）。
    struct AchievementEntry
    {
        int m_field0;
        int m_flag;      // +4（ORIG 读点）
        int m_field8;
    };
    struct AchievementView
    {
        char m_pad[0xb1a0];
        AchievementEntry m_entries[0x200];
    };
    for (int i = 0; i <= 0x1ff; ++i)                       // i <= 0x1ff（cmpl 0x1ff setle）
    {
        if (((AchievementView*)sig)->m_entries[i].m_flag == 0)
        {
            break;                                          // 计数停在首个零条目
        }
        ++count;
    }
    sig->m_achievement.m_count = count;             // +0xc9a4（movl）

    // ORIG 0x084065c4：get_uint 目标直接是 sig+0xc9a8（无 lastUpdate 局部，P0 槽律）。
    if (!db->get_uint(1, sig->m_achievement.m_saveTime))
    {
        return false;
    }
    return true;
}