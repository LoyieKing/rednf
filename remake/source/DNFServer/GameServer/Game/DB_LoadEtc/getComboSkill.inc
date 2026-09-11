// ============================================================================
// DB_LoadEtc::getComboSkill @0x0844a704 —— 每行 7 个 short 填一个 comoboSkillData，
// 行数上限 min(count,6)；成功返回 1、任何 get 失败行号错误路径返回 0。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x0844a704（orig=124/ours=124，槽全等）。
// 已修（本波）：①循环改标准 `for (row = 0; row < count && row < 6; ++row)` + 尾 `return true;`
//   ⇒ 4.4 复现 ORIG 的「入口 jmp 底部条件块 + 短路物化 mov$1/$0」（P283/P214①）；
//   原 `for(;;)+if(!(…))return true` 使条件置顶（DIFF）。
//   ②去 slot 具名指针局部，且**关键**：地址式由 `(comoboSkillData*)((char*)out + row*0xe)`
//   改**数组下标 `out[row]`**（sizeof(comoboSkillData)==0xe，pack(1)）——指针 cast 形被 4.4
//   缓存到 -0x14 临时槽（row 被挤到 -0x18、三处 trace 槽整体浅 4）；下标形每次重算
//   地址（P183 地址重物化），row=-0x14/col=-0x10/k=-0xc/trace -0x44/-0x34/-0x24 全等。
//   ③k 循环 `k <= 5`（ORIG 尾部 setle 形，P51）。
// 已修（前波）：col 改 0 起取后自增（ORIG -0x10）+ 三处 trace 临时链。
bool DB_LoadEtc::getComboSkill(MySQL* db, unsigned int accId, int count,
                               comoboSkillData* out)
{
    if (db == 0 || out == 0)
    {
        return false;
    }
    for (int row = 0; row < count && row < 6; ++row)
    {
        if (!db->fetch())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbb01, 5)(
                "DB_LoadEtc::getComboSkill fetch, exec() ERROR charac_no=%u",
                accId);
            return false;
        }
        // P183 地址重物化：ORIG 每次重发 row*0xe 全链（无 slot 具名局部缓存）。
        int col = 0;                                          // ORIG -0x10 起 0，取后自增
        if (!db->get_short(col++, out[row].m_comboSkillIndex))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbb0a, 5)(
                "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u", col,
                accId);
            return false;
        }
        for (int k = 0; k <= 5; ++k)
        {
            if (!db->get_short(col++, out[row].m_comboSkillList[k]))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xbb14, 5)(
                    "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u",
                    col, accId);
                return false;
            }
        }
    }
    return true;
}