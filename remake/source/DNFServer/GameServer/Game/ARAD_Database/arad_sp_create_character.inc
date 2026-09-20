// ORIG 0x0818af9a：inSert into charac_info ……（新角色主记录，含两张 blob）。
// 列名/占位与字段映射完全照抄 ORIG 格式串与实参顺序。
// b28d-solo-ARAD_Database verdict=DIFF(193/195) N 封顶：P167 调用点集 8/8 CLEAR；
//   残余=槽组整体平移（ORIG escName -0x5c / spill -0xac..-0x7c vs 4.4 -0x98/-0x58..，
//   N82/N95 槽分配策略）+ maxHP/maxMP 无槽（ORIG 用 %esi/%edi）+ insert 返回值尾部
//   双 return 块序（ORIG Bfalse-ret0 在前，4.4 恒 Btrue-ret1 在前，N93附）。语义闭合。
// [S5-2026-09-11 复攻] gap −2→**0**（195→193，ndiff 154→144）：删 maxHP/maxMP 两个
//   具名局部（ORIG 0x0818b168 `mov 0x26(%eax),%esi` / 0x0818b223 `mov %esi,0x18(%esp)`
//   实证无槽、寄存器直传），实参位直接展开 SIG_ATTR ⇒ 4.4 亦不再建槽往返（P0 正例）。
//   残余 `je`(ORIG)/`jne`(OURS) 1 条 = N93附 尾块序；槽组平移 = N82/N95（编译器分配
//   策略差：ORIG push %esi/%edi 保活、4.4 用栈槽）。语义闭合。
int CREATE_QUERY::arad_sp_create_character(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (arad_sp_create_character_check(db, sig) != 0)
    {
        return -1;
    }

    // ORIG：库名 = G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)2].m_db(+0x42)，
    // 具名直访 public m_dbConnInfo（+0x68）内联进 name_check 第三实参（ORIG 无
    // connMap/dbName 具名槽；operator[] 的 key 临时槽 -0x20 由右值实参自然产生）。
    if (arad_sp_charater_name_check(
            db, sig,
            G_CEnvironment()->m_dbConnInfo[
                static_cast<ENUM_DB_HANDLE_IDX>(2)].m_db) != 0)
    {
        return -1;
    }

    char escName[60] = {0};                          // ORIG 0x818b015 rep stos 60B
    db->escape_string(escName, sig->m_name);

    // 【ABI 镜像】charac 主属性打包区（+0x26..+0x79 内散列整型，按报告字节宽读取）。
    // ORIG 0x818b4a9 起为 insert 实参右→左求值 spill：具名局部按 ORIG 槽深→浅
    // （-0xac..-0x6c）顺序声明，4.4 全头部声明同向分配即逐槽对齐。
    int expertJob        = static_cast<int>(static_cast<char>(sig->m_pad25)); // -0xac
    int maxFatigue       = DEFAULT_MAX_FATIGUE;                              // -0xa8
    unsigned int chaWt   = SIG_ATTR(sig, m_chaWt);                           // -0xa4
    int jump             = static_cast<short>(SIG_ATTR(sig, m_jump));        // -0xa0
    int hitRecov         = static_cast<short>(SIG_ATTR(sig, m_hitRecov));    // -0x9c
    unsigned int castSpd = SIG_ATTR(sig, m_castSpd);                         // -0x98
    unsigned int atkSpd  = SIG_ATTR(sig, m_atkSpd);                          // -0x94
    unsigned int moveSpd = SIG_ATTR(sig, m_moveSpd);                         // -0x90
    int mpRegen          = static_cast<short>(SIG_ATTR(sig, m_mpRegen));     // -0x8c
    int hpRegen          = static_cast<short>(SIG_ATTR(sig, m_hpRegen));     // -0x88
    unsigned int invenWt = SIG_ATTR(sig, m_invenWt);                         // -0x84
    // blob 序列化（ORIG 0x0818b0fb/0x0818b123 调用、0x0818b1f5/0x0818b1fc 压栈实证）：
    // 列序 element_resist(第11实参) ← blob_to_str(db,0,sig+0x36,8)＝-0x7c；
    //      spec_property(第12实参) ← blob_to_str(db,1,sig+0x3e,0x22)＝-0x80。
    // 故源码先求值 spec_property(+0x3e,0x22)、后求值 element_resist(+0x36,8)（cdecl 右→左）。
    char* specProperty   = db->blob_to_str(1, (char*)SIG_ATTR(sig, m_specProperty), 0x22); // -0x80
    char* elementResist  = db->blob_to_str(0, (char*)SIG_ATTR(sig, m_elementResist), 8);    // -0x7c
    unsigned int magDef  = SIG_ATTR(sig, m_magDef);                          // -0x78
    unsigned int magAtk  = SIG_ATTR(sig, m_magAtk);                          // -0x74
    unsigned int phyDef  = SIG_ATTR(sig, m_phyDef);                          // -0x70
    unsigned int phyAtk  = SIG_ATTR(sig, m_phyAtk);                          // -0x6c
    int ret = db->insert(
            "inSert into charac_info "
            "( m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,phy_defense,mag_attack,"
            "mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,"
            "move_speed,attack_speed,cast_speed,hit_recovery,jump,charac_weight,"
            "max_fatigue,create_time,expert_job) "
            "values(%u,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,"
            "%d,%d,NOW(),%d)",
            sig->m_accId,
            escName,
            static_cast<int>(static_cast<char>(sig->m_slot)),            // +0x22
            static_cast<int>(static_cast<char>(sig->m_ssnCheckFail)),    // +0x534c
            SIG_ATTR(sig, m_maxHP), SIG_ATTR(sig, m_maxMP), phyAtk, phyDef, magAtk, magDef,
            elementResist, specProperty,
            invenWt, hpRegen, mpRegen, moveSpd, atkSpd, castSpd, hitRecov, jump,
            chaWt, maxFatigue, expertJob);
    // [round5 C05-F1 三形复验，判 N93附 维持封顶] ORIG 0x0818b25d 尾块序 =
    //   `cmpl $0,-0x1c(%ebp); je <成功块>; mov $0xffffffff,%eax; jmp <尾>; <成功>:
    //   mov $0x0,%eax`（失败直落、成功块落函数尾）。三形实测：
    //   ① 平铺 `if (ret==0){return 0;} return -1;` ⇒ 4.4 反序 `jne <失败块>; mov $0; jmp`；
    //   ② `goto ok_ret;` 尾标签形 ⇒ **极性/块序翻正**（`je <成功块>; mov $-1; jmp`）但多发
    //     1 条对齐 nop（指令数 +1 = 退化）⇒ 按 C05③/A28 边界回退；
    //   ③ P47 三目 `return (ret==0)?0:-1;` ⇒ 同 ①（反序）。
    //   TU 四数三形恒 193/0/6/13（无净收益）⇒ 取最小改动形（①），N93附 封顶。
    if (ret == 0)
    {
        return 0;
    }
    return -1;
}