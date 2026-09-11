// ============================================================================
// DB_LoadEtc::LoadComboSkill @0x0844a47a —— 单树连招装载（kind∈{0,1}）。
// ============================================================================
// [b29-Lane21] 残差 6 行 = N76 store 装载序：ORIG `mov -0xc(%ebp),%eax; mov %eax,%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（4 条，RHS-first + 中转）vs 4.4 `mov -0xc(%ebp),%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（3 条）；视图/成员/两值形已试 ⇒ N76 封顶。
// [b28d-solo-DB_LoadEtc] N封顶（N76 store 装载序）：ORIG 0x0844a47a。
// 已修（本波）：①**真 bug（P92/P196）**：getComboSkill 第 2 实参原写 sig->m_accId（+0），
//   ORIG 0x0844a5a4 为 `mov 0x4(%eax),%eax` = sig->m_characNo（+4）——该值即
//   getComboSkill 日志的 charac_no=%u，原码打印错误账号。
//   ②去 cs 具名局部（P183）+ 改成员视图 ComboSkillView（pad 0x14aae + tree1Count/tree2Count/
//   tree1Data[6]/tree2Data[6]）⇒ ORIG disp 形 `mov %dl,0x14aae(%eax)` / `lea 0x14ab4(%eax),%edx`
//   （cast 视图恒 `add $0x14aaa` + disp4，N107）。
//   ③四段改 **if / else-if** 链（count 写与两处调用各自成对）⇒ 复现 ORIG 的 `jmp` 块尾 +
//   `cmp $1` elif 布局（全独立 if 时 4.4 直落无 jmp）。
// 残差（不可翻，N76）：count 写点 store 装载序——ORIG `mov -0xc,%eax; mov %eax,%edx;
//   mov sig,%eax; mov %dl,disp`（RHS-first + eax→edx 中转）vs 4.4 恒 LHS-first 直存
//   `mov sig,%eax; mov -0xc,%edx; mov %dl,disp`；字段 unsigned char 与 int RHS 本已异型，
//   P211 两旋钮（字段/callee）无第三档可用。
//   [b28d-solo 复测] 去 `(unsigned char)` cast（`m_tree1Count = nRows;`）产物**完全相同**
//   （4.4 视 uchar←int 为可直存，P211 档③不适用）⇒ 维持去 cast 形；p167 CLEAR 15/15。
// 已修（前波）：去 kindVal 局部（P0 直比形参）+ exec 具名 ok + 四处 trace 临时链。
bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC* sig, ENUM_SKILL_TREE_KIND kind)
{
    // ORIG 0x0844a47a：kind 无具名槽（test/jne + cmp $1/je 直比形参，P0 槽律）。
    if (!(kind == 0 || kind == 1))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbac4, 5)(
            "DB_LoadEtc::LoadComboSkill kind(%d) error characNo(%u)", kind,
            sig->m_characNo);
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        return false;
    }
    db->set_query("seLect combo_idx, value1, value2, value3, value4, value5,"
                  " value6 from %s where charac_no=%u",
                  getComboSkillTableName(kind), sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbad4, 5)(
            "DB_LoadEtc::LoadComboSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }

    // 树条目计数写回：kind0→+0x14aae（tree1Count）、kind1→+0x14aaf；
    // ORIG 对两 kind 各自独立调用 getComboSkill（0x844a5b2 / 0x844a614）。
    // 真 bug 修正（P92/P196）：ORIG 0x0844a5a4 实参是 `mov 0x4(%eax),%eax` = sig->m_characNo，
    // 原写 sig->m_accId（+0）⇒ getComboSkill 日志 charac_no 打印错误值。
    // P183：无 cs 具名局部（ORIG 每次重发 sig+0x14aaa 全链，槽 -0x14 起不漂移）。
    // 成员视图直接命中 ORIG 的 disp 形（`mov %dl,0x14aae(%eax)`），避免 cast 视图
    // `add $0x14aaa,%eax` + disp4（N107）。
    // [b29-Lane21 实测] Main「视图 cast 反模式」洞察在本件**反向**：改权威类型
    // `reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)->m_tree1Count = nRows;`
    // ⇒ 4.4 产 `add $0x14aaa,%eax; mov -0xc,%edx; mov %dl,0x4(%eax)`（disp 被拆成两段），
    // ndiff 6→18 反劣，已回退（本件真差异是 store 装载序 LHS/RHS，非 disp/N107）。
    struct ComboSkillView
    {
        char m_pad[0x14aae];
        unsigned char m_tree1Count;      // +0x14aae（N76：P211 档③不适用——字段与
        unsigned char m_tree2Count;      // +0x14aaf  RHS 本已异型，signed/unsigned 同形）
        char m_pad2[4];                  // +0x14ab0
        comoboSkillData m_tree1Data[6];  // +0x14ab4（0x54）
        comoboSkillData m_tree2Data[6];  // +0x14b08（0x54）
    };
    if (kind == 0)
    {
        // ORIG 0x0844a5c7：`mov -0xc(%ebp),%eax; mov %eax,%edx; mov 0xc(%ebp),%eax;
        //   mov %dl,0x14aae(%eax)`（RHS-first + eax→edx 中转，N76 封顶）。
        ((ComboSkillView*)sig)->m_tree1Count = (char)nRows;
    }
    else if (kind == 1)
    {
        ((ComboSkillView*)sig)->m_tree2Count = (char)nRows; // 同上（ORIG 0x0844a5e0）
    }
    if (kind == 0)
    {
        if (!getComboSkill(db, sig->m_characNo, nRows,
                ((ComboSkillView*)sig)->m_tree1Data))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbae7, 5)(
                "DB_LoadEtc::LoadComboSkill, getComboSkill ERROR charac_no=%u",
                sig->m_characNo);
        }
    }
    else if (kind == 1)
    {
        if (!getComboSkill(db, sig->m_characNo, nRows,
                ((ComboSkillView*)sig)->m_tree2Data))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbaef, 5)(
                "DB_LoadEtc::LoadComboSkill, getComboSkill ERROR charac_no=%u",
                sig->m_characNo);
        }
    }
    return true;
}