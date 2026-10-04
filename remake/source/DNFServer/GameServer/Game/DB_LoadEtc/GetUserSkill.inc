// ============================================================================
// DB_LoadEtc::GetUserSkill @0x084075d2 —— 主 skill 行 + tag 双槽位 blob。
// §6.2 语义收口：12 列 get/blob 链、col 槽（-0x10）col++ 形、fail 岛行号
// （0x1d50..0x1df1）与 ORIG 对齐；AE 天花板=N82 trace 槽方向三证（Main
// batch-17 裁定按语义收口，不投纯文本序差）。
// 列序 12 列，目标偏移照汇编；n_rows==0 且 PvP 技能树频道时补插一行。
// 错误行号 0x1d50/0x1d5d/0x1d69/0x1d71/0x1d98/0x1dac/0x1db2/0x1dea/0x1df1。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x084075d2；p167 CLEAR（0/0），
// 残差为帧/槽分配（ORIG 478 vs 4.4 468）。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=624 (orig=478/ours=468)：N82 帧/槽错位（12 列 get/blob
// 链的 trace 槽方向）+ N89 镜像（get_compressed_blob_data 内联差）；p167 CLEAR 0/0 ⇒ 语义完成。
// [b28d-SweepDBLoadEtc 本波] 真 bug 修复：①col 初值 1→0（ORIG 0x084077e9 `movl $0x0,-0x10`
// + 首列 `get_uint(col++)`；旧 col=1 使后续 blob 列号整体 +1）；②去 sk 指针/spVals 局部（P183，
// ORIG 每处单条 lea/add disp 直写成员，改 m_mixedRegion[off] 常量索引）；③scriptVersion 直传
// 成员地址（ORIG `lea 0xcdd1(%eax),%edx`）；④tag 段 col 变量（ORIG -0xc）；⑤9 处 trace 改
// P227 全表达式临时链；⑥exec/fetch/get_uint 共 9 处改具名 ok 槽（ORIG -0x15）。
// 读数：gap +10（478/468）→ +1（478/477）；p167 46=46（仅 cMyTrace C1/C2 别名）；
// 残差=add+mov↔lea 跨版本（N107/P30）。
// [b29-Lane44 判据①复扫·已归零] 上记 9 处 add+mov↔lea 实为**可翻**（旧判 N107 系探针形不全）：
//   4.4 对「cast 取址 `(T*)&arr[i]`」恒产 `add $K,%eax; mov %eax,%edx`，对「视图成员/
//   指针算术/数组名退化」恒产单条 `lea K(%eax),%edx`；4.1 恰好相反 ⇒ 两侧互补，改我方表达式
//   形即可逐点对齐（探针 /tmp/bl_probe.cpp~bl_probe5.cpp，447/446 后端同形）：
//   ①ORIG lea 的 4 点（+0xc9c8/+0xcb64/+0xcd00/+0xcd04）→ 局部 trivial 镜像视图
//     `SkillMixedView` 具名成员取址（类型匹配、无 cast）；
//   ②ORIG `add K; mov %eax,%edx` 的 5 点（+0xc9cc/+0xcb68/+0xcd08/+0x29fd/+0x2b97，均为
//     数组名退化/指针算术）→ 整型加法形 `(char*)((unsigned int)sig + K)`（P319 同族，
//     32 位指针≡int，语义等价）。
//   读数：478/477（ndiff 18，9 hunk）→ **478/478 全等**（verdict AE 54，TU diff 12→11）。
bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc12GetUserSkill…E9handleIdx
    bool ok;                           // ORIG -0x15 具名槽（深于 db=-0x14，P212）
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect remain_sp,skill_slot,remain_sp_2nd, skill_slot_2nd,"
                  " remain_sfp_1st, remain_sfp_2nd, skill_command, skill_slot_lethe,"
                  " lethe_flag, skill_slot_lethe_2nd, lethe_flag_2nd,script_version"
                  " from %s where charac_no=%u",
                  get_skill_table_name(false), sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d50, 5)(
            "DB_LoadSkill::GetUserSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    if (db->get_n_rows() == 0 && G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG：缺行 + PvP 技能树频道 → inSert 占位行并置 0xcdd4 标志。
        db->set_query("inSert into %s (charac_no) values(%u)",
                      get_skill_table_name(false), sig->m_characNo);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1d5d, 5)(
                "DB_LoadSkill::GetUserSkill insert, exec() ERROR"
                " charac_no=%u", sig->m_characNo);
            return false;
        }
        sig->m_pvpSkillInsertFlag = 1;                       // +0xcdd4 movb
        return true;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d69, 5)(
            "DB_LoadSkill::GetUserSkill, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    int col = 0;                                             // ORIG -0x10 从 0 起
    // 混合区首 0x414 为 SIG_LOAD_SKILL 视图（+0xc9c0；字段名见 SigTypes_C.h，
    // 证据 GetSkill.md/SkillSlot 透传布局：+0x8/+0xc/+0x1a4/+0x1a8/+0x340/
    // +0x344/+0x348 = sig+0xc9c8/0xc9cc/0xcb64/0xcb68/0xcd00/0xcd04/0xcd08）。
    // P183/P30：ORIG 每处单条 lea/add disp（+0xc9c8/+0xc9cc/... 折叠），禁 sk 指针
    // 缓存、禁 (SIG_LOAD_SKILL*)sig->m_mixedRegion->field（4.4 拆 add $0xc9c0;add $disp）。
    // 常量索引与 SIG_LOAD_SKILL 视图字段一一对应（+0x8/+0xc/+0x1a4/+0x1a8/+0x340/
    // +0x344/+0x348）。
    // [Lane44-新判据①] ORIG 0xc9c8/0xcb64/0xcd00/0xcd04 为单条 `lea K(%eax),%edx`
    //   （视图成员取址形），4.4 对 `(unsigned int*)&arr[i]` 恒产 `add $K,%eax; mov`⇒ 改用
    //   局部 trivial 镜像视图成员取址（探针实测 4.4 产单条 lea），四字段偏移见下。
    ok = db->get_uint(col++,
                      reinterpret_cast<unsigned int&>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_remainSpTree1));
    if (!ok)
    {                                                        // col0 remain_sp
        cMyTrace(__PRETTY_FUNCTION__, 0x1d71, 5)(
            "DB_LoadSkill::GetUserSkill, get(0) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  reinterpret_cast<char*>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_skillSlotTree1),
                                  0x198, 2))
    {                                                        // skill_slot
        return false;
    }
    ok = db->get_uint(col++,
                      reinterpret_cast<unsigned int&>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_remainSpTree2));
    if (!ok)
    {                                                        // remain_sp_2nd→0xcb64
        cMyTrace(__PRETTY_FUNCTION__, 0x1d98, 5)(
            "DB_LoadSkill::GetUserSkill, get(4) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  reinterpret_cast<char*>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_skillSlotTree2),
                                  0x198, 2))
    {                                                        // skill_slot_2nd
        return false;
    }
    ok = db->get_uint(col++,
                      reinterpret_cast<unsigned int&>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_remainSfpTree1));
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dac, 5)(
            "DB_LoadSkill::GetUserSkill, get(7) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++,
                      reinterpret_cast<unsigned int&>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_remainSfpTree2));
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1db2, 5)(
            "DB_LoadSkill::GetUserSkill, get(8) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  reinterpret_cast<char*>(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion)->m_commands),
                                  200, 4))
    {                                                        // skill_command
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  reinterpret_cast<char*>(&sig->m_backupSkill), 0x198, 2))
    {                                                        // skill_slot_lethe
        return false;
    }
    int letheFlag = 0;                                       // ORIG -0xac 列8
    if (!db->get_int(col++, letheFlag))                      // lethe_flag(int)
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  reinterpret_cast<char*>(&sig->m_backupSkill2ND), 0x198, 2))
    {                                                        // skill_slot_lethe_2nd
        return false;
    }
    int letheFlag2 = 0;                                      // ORIG -0xb0 列10
    if (!db->get_int(col++, letheFlag2))                     // lethe_flag_2nd(int)
    {
        return false;
    }
    if (!db->get_ubyte(col++, sig->m_scriptVersion))         // script_version→+0xcdd1
    {
        return false;
    }

    // lethe 标志低位字节落位（ORIG movzbl/mov dl）。
    sig->m_backupSkill.m_pad196[0] = (unsigned char)letheFlag;            // +0x2b95
    sig->m_oneDayLetheFlag = (unsigned char)letheFlag;               // +0x29fc
    sig->m_backupSkill2ND.m_pad196[0] = (unsigned char)letheFlag2;        // +0x2d2f
    sig->m_oneDayLetheFlag2ND = (unsigned char)letheFlag2;           // +0x2b96

    if (sig->m_fieldC != 0)
    {
        // tagCharacNo 存在 → 拉主/tag 双槽位 blob。
        db->set_query("seLect skill_slot, skill_slot_2nd from skill where"
                      " charac_no=%u", sig->m_fieldC);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1dea, 5)(
                "DB_LoadSkill::GetUserSkill, exec() ERROR tagCharacNo=%u",
                sig->m_fieldC);
            return false;
        }
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1df1, 5)(
                "DB_LoadSkill::GetUserSkill, fetch() ERROR tagCharacNo=%u",
                sig->m_fieldC);
            return false;
        }
        int col = 0;                                        // ORIG -0xc 块内槽
        if (!get_compressed_blob_data(db, col++,
                                      reinterpret_cast<char*>(&sig->m_tagEtc),
                                      0x198, 2))
        {
            return false;
        }
        if (!get_compressed_blob_data(db, col++,
                                      reinterpret_cast<char*>(sig->m_tagEtc.m_skills[1]),
                                      0x198, 2))
        {
            return false;
        }
    }
    return true;
}