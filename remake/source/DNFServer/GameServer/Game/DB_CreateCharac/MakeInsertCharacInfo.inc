// b28d-solo-DB_CreateCharac verdict=DIFF(N7 封顶) ORIG 0x080ec2a0 orig/ours=154/153。
// 已收：①blob_to_str 调用序（先 col=1 spec 后 col=0 elem）②P183 去 jv 指针缓存（每个字段
// 重读 sig 形参 `mov 0x10(%ebp),%eax; mov off(%eax)`，与 ORIG 逐条一致）③maxHP 不设局部、
// 实参内联（%edi 承载，push/pop %edi 与帧 0xcc 归位）④+0x25 改具名局部 -0x60、+0x534c 内联
// （ORIG 前者有槽、后者 %esi 无槽）⑤帧/槽位/-0x64..-0x1c 全对齐、%edi/%esi/%ebx 角色一致。
// 残余 = 1 条：ORIG `call NumberToString; mov %eax,%edx`（结果移 %edx，压栈用 %eax 中转），
// 4.4 恒留 %eax、改用 %edx 中转压栈 ⇒ 助记符序列全等仅中转寄存器角色互换（N7 族；试过
// P226 裸语句表达式/具名局部/register 提示/cast 四形，均不翻）。语义/callsite 完整。
void DB_CreateCharac::MakeInsertCharacInfo(MySQL* db, SIG_CREATE_CHARAC* sig,
                                           char const* escapedName)
{
    int eventCharacLevel = (signed char)sig->m_field23;   // +0x23 列 event_charac_level
    // ORIG +0x25 值有具名槽 -0x60（P0：槽往返 ⇒ 有局部）；+0x534c 无槽（%esi 承载）⇒
    // 两者处理互换。
    int sexAt0x25 = (signed char)sig->m_pad25;            // +0x25 ← ORIG -0x60 槽
    int fatigueMax = DEFAULT_MAX_FATIGUE;

    // P183：ORIG 每个字段都重读 sig 形参（mov 0x10(%ebp),%eax; mov off(%eax),%eax）
    // ⇒ 禁指针缓存，逐点内联视图 cast。
    unsigned int expertJob   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_expertJob);   // +0x74
    int jumpValue            = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_jump);                 // +0x72
    int hitRecovery          = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_hitRecovery);          // +0x70
    unsigned int castSpeed   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_castSpeed);   // +0x6e
    unsigned int attackSpeed = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_attackSpeed); // +0x6c
    unsigned int moveSpeed   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_moveSpeed);   // +0x68
    int mpRegenShort         = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_mpRegen);              // +0x66
    int hpRegenShort         = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_hpRegen);              // +0x64
    unsigned int invenWeight = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_invenWeight); // +0x60
    // 第一次 blob_to_str(col=1, sig+0x3e, 0x22) → spec_property '%s'（ORIG 调用序在前）
    char const* specPropertyBlob =
        db->blob_to_str(1, const_cast<unsigned char*>(reinterpret_cast<MiciJobView const*>(sig)->m_specProperty), 0x22);
    // 第二次 blob_to_str(col=0, sig+0x36, 8) → element_resist '%s'
    char const* elementResistBlob =
        db->blob_to_str(0, const_cast<unsigned char*>(reinterpret_cast<MiciJobView const*>(sig)->m_elementResist), 8);
    unsigned int magDefense = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_magDefense);   // +0x34
    unsigned int magAttack  = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_magAttack);    // +0x32
    unsigned int phyDefense = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_phyDefense);   // +0x30
    unsigned int phyAttack  = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_phyAttack);    // +0x2e
    unsigned int maxMP      = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_maxMp);        // +0x2a
    // P0：ORIG maxHP 无栈槽（080ec3bf 装载入 %edi 跨 NumberToString 保活）⇒
    // 不设具名局部，实参处内联表达式。

    db->set_query(
        "inSert into charac_info (m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,"
        "phy_defense,mag_attack,mag_defense,element_resist,spec_property,"
        "inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,"
        "hit_recovery,jump,charac_weight,max_fatigue,create_time,expert_job,"
        "event_charac_level) values(%s,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW(),%d,%d)",
        NumberToString(sig->m_accId, 0), escapedName,
        (int)(signed char)sig->m_slot,     // job 列 ← sig+0x22（ORIG %ebx）
        (signed char)sig->m_ssnCheckFail,  // sex 列 ← sig+0x534c（ORIG %esi 无槽）
        static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_maxHp),  // +0x26（ORIG %edi）
        maxMP, phyAttack, phyDefense, magAttack, magDefense,
        elementResistBlob, specPropertyBlob,
        invenWeight, hpRegenShort, mpRegenShort, moveSpeed, attackSpeed,
        castSpeed, hitRecovery, jumpValue, expertJob, fatigueMax,
        sexAt0x25,                         // expert_job 列 ← sig+0x25（ORIG -0x60 槽）
        eventCharacLevel);                 // event_charac_level ← sig+0x23
}