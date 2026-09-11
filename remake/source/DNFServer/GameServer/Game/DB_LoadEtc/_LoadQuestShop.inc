// ============================================================================
// DB_LoadEtc::_LoadQuestShop @0x08404b72（static）
// get fail 行号序列：qp/init_count 前 2 次 → cMyTrace ret0（0x17ea/0x17f1）；
// 自 max_hp 起 throw int(line)：0x17f7..0x1811、questPiece 0x1816（跳过 0x1810）。
// catch 后 memset(param_2+0xc,0,0x9a) 并 logFormat 行 0x181f。
// unroll 转录：列号用 local_14(idx) 取后自增（非字面量常数），throw 就地展开
// （__cxa_allocate_exception/__cxa_throw 逐列内联，非辅助函数），exec/fetch/
// qp/init_count 结果经 local_19(ret) 中转 → 指令数 460→690，与 ORIG 690 一致。
// [b28c-lane17] fresh 复验：690=690、助记符序列全等 ⇒ NEAR（残差全为槽号/
// callee-saved 分配差，N82 族——4.1 不按 lifetime 复用栈槽使全帧槽号错位，
// ndiff 与语义完成度反向）。按 §6.2 三件套口径判「语义完成、byte 层 N 封顶」。
// ============================================================================
bool DB_LoadEtc::_LoadQuestShop(unsigned int characNo, CHARAC_QUEST_SHOP_DATA* out)
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc14_LoadQuestShop…E9handleIdx
    bool ret;                   // ORIG local_19 @-0x15(%ebp)：exec/fetch/qp/init_count
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    int idx;                    // ORIG local_14 @-0x10(%ebp)：列号，随每次 get 自增
    db->set_query("seLect qp, init_count, max_hp, max_mp, psy_attack, psy_defense,"
                  " mag_attack, mag_defence, move_speed, attack_speed, hp_regen,"
                  " mp_regen, all_element_resist, fire_element_resist,"
                  " water_element_resist, light_element_resist, dark_element_resist,"
                  " all_element_attack,fire_element_attack, water_element_attack,"
                  " light_element_attack, dark_element_attack, psy_critical,"
                  " mag_critical, good_hit , evasion , hit_recovery ,"
                  " separate_psy_mag_attack , quest_piece from charac_quest_shop"
                  " where charac_no=%u", characNo);
    ret = db->exec(true);
    if (!ret)
    {
        return false;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return true;
    }
    ret = db->fetch();
    if (!ret)
    {
        return false;
    }

    // 列序/目标字段/失败抛出行号逐项对齐汇编（local_14 自 0 起、取后自增）。
    idx = 0;
    ret = db->get_uint(idx++, out->m_qp);                       // qp          +0x04
    if (!ret)
    {
        // b28d-31: P221 全表达式临时链（ORIG 0x08404bff 槽 -0x38 在 trq2 的
        // -0x28 之前；具名声明 4.4 槽序反向 ⇒ 改构造即调用）。
        cMyTrace(__PRETTY_FUNCTION__, 0x17ea, 5)(
            "DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", idx,
            characNo);
        return false;
    }
    ret = db->get_uint(idx++, out->m_initCount);                // init_count  +0x08
    if (!ret)
    {
        // b28d-31: P221 全表达式临时链（ORIG 0x08404c22 槽 -0x28）。
        cMyTrace(__PRETTY_FUNCTION__, 0x17f1, 5)(
            "DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", idx,
            characNo);
        return false;
    }
    // ORIG EH 区间自首个 throw 起：仅逐列 get_ushort/get_uint 段位于 try 内。
    try
    {
#       define QUESTSHOP_USHORT(field, line_)                                  \
        do                                                                     \
        {                                                                      \
            if (!db->get_ushort(idx++, out->field))                            \
            {                                                                  \
                throw line_;                                                   \
            }                                                                  \
        } while (0)

        QUESTSHOP_USHORT(m_data.m_maxHp, 0x17f7);                   // max_hp           +0x26
        QUESTSHOP_USHORT(m_data.m_maxMp, 0x17f8);                   // max_mp           +0x6c
        QUESTSHOP_USHORT(m_data.m_psyAttack, 0x17f9);               // psy_attack       +0x0c
        QUESTSHOP_USHORT(m_data.m_psyDefense, 0x17fa);              // psy_defense      +0x12
        QUESTSHOP_USHORT(m_data.m_magAttack, 0x17fb);               // mag_attack       +0x0e
        QUESTSHOP_USHORT(m_data.m_magDefence, 0x17fc);              // mag_defence      +0x10
        QUESTSHOP_USHORT(m_data.m_moveSpeed, 0x17fd);               // move_speed       +0x22
        QUESTSHOP_USHORT(m_data.m_attackSpeed, 0x17fe);             // attack_speed     +0x20
        QUESTSHOP_USHORT(m_data.m_hpRegen, 0x17ff);                 // hp_regen         +0x28
        QUESTSHOP_USHORT(m_data.m_mpRegen, 0x1800);                 // mp_regen         +0x46
        QUESTSHOP_USHORT(m_data.m_allElementResist, 0x1801);        // +0x3c
        QUESTSHOP_USHORT(m_data.m_fireElementResist, 0x1802);       // +0x34
        QUESTSHOP_USHORT(m_data.m_waterElementResist, 0x1803);      // +0x36
        QUESTSHOP_USHORT(m_data.m_lightElementResist, 0x1804);      // +0x3a
        QUESTSHOP_USHORT(m_data.m_darkElementResist, 0x1805);       // +0x38
        QUESTSHOP_USHORT(m_data.m_allElementAttack, 0x1806);        // +0x68
        QUESTSHOP_USHORT(m_data.m_fireElementAttack, 0x1807);       // +0x60
        QUESTSHOP_USHORT(m_data.m_waterElementAttack, 0x1808);      // +0x62
        QUESTSHOP_USHORT(m_data.m_lightElementAttack, 0x1809);      // +0x66
        QUESTSHOP_USHORT(m_data.m_darkElementAttack, 0x180a);       // +0x64
        QUESTSHOP_USHORT(m_data.m_psyCritical, 0x180b);             // +0x2a
        QUESTSHOP_USHORT(m_data.m_magCritical, 0x180c);             // +0x2c
        QUESTSHOP_USHORT(m_data.m_goodHit, 0x180d);                 // +0x4e
        QUESTSHOP_USHORT(m_data.m_evasion, 0x180e);                 // +0x42
        QUESTSHOP_USHORT(m_data.m_hitRecovery, 0x180f);             // +0x48
        QUESTSHOP_USHORT(m_data.m_separatePsyMagAttack, 0x1811);    // +0x76（跳过 0x1810）

#       undef QUESTSHOP_USHORT

        if (!db->get_uint(idx++, out->m_questPiece))            // quest_piece +0xa6
        {
            throw 0x1816;
        }
    }
    catch (int e)
    {
        memset(&out->m_data, 0, sizeof(out->m_data));
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x181f,
                              "DB_LoadEtc::LoadQuestShop, get(%d) ERROR charac_no=%u",
                              idx /*ORIG -0x10(%ebp)=local_14 列号*/, characNo);
        return false;
    }
    return true;
}