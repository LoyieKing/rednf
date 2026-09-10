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
    struct SkillMixedView
    {
        char           m_pad[0xc9c8];
        unsigned int   m_remainSp;         // +0xc9c8（ORIG lea 形）
        char           m_pad2[0x198];      // +0xc9cc..0xcb64
        unsigned int   m_remainSp2nd;      // +0xcb64（ORIG lea 形）
        char           m_pad3[0x198];      // +0xcb68..0xcd00
        unsigned int   m_field340;         // +0xcd00（ORIG lea 形）
        unsigned int   m_field344;         // +0xcd04（ORIG lea 形）
    };
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_remainSp);
    if (!ok)
    {                                                        // col0 remain_sp
        cMyTrace(__PRETTY_FUNCTION__, 0x1d71, 5)(
            "DB_LoadSkill::GetUserSkill, get(0) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xc9cc),
                                  0x198, 2))
    {                                                        // skill_slot
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_remainSp2nd);
    if (!ok)
    {                                                        // remain_sp_2nd→0xcb64
        cMyTrace(__PRETTY_FUNCTION__, 0x1d98, 5)(
            "DB_LoadSkill::GetUserSkill, get(4) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xcb68),
                                  0x198, 2))
    {                                                        // skill_slot_2nd
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_field340);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dac, 5)(
            "DB_LoadSkill::GetUserSkill, get(7) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_field344);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1db2, 5)(
            "DB_LoadSkill::GetUserSkill, get(8) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xcd08),
                                  200, 4))
    {                                                        // skill_command
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0x29fd), 0x198, 2))
    {                                                        // skill_slot_lethe
        return false;
    }
    int letheFlag = 0;                                       // ORIG -0xac 列8
    if (!db->get_int(col++, letheFlag))                      // lethe_flag(int)
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0x2b97), 0x198, 2))
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
    sig->m_backupSkill[0x198] = (unsigned char)letheFlag;            // +0x2b95
    sig->m_oneDayLetheFlag = (unsigned char)letheFlag;               // +0x29fc
    sig->m_backupSkill2ND[0x198] = (unsigned char)letheFlag2;        // +0x2d2f
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
