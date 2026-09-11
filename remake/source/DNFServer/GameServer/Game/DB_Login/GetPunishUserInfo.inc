// [SweepDBLoadEtcMain 本波] 旧「N18a/P211 封顶」**作废**，DIFF→AE：写入侧改走
// SigTypes_D.h 匿名 union 的 `m_punishInfoCountU` unsigned 视角（异型 ⇒
// RHS-first+edx 中转），复现 ORIG 08410da9 `mov %eax,%edx; mov 0xc(%ebp),%eax;
// mov %edx,0x3a50(%eax)`（探针 /tmp/punp.cpp f↔g）；消费侧 int 视角零变化
// （A/B 实测 ARAD_LoginSecurity 53/100/2/3/49、InterDispatcher 121/1331/5/133/700
// union 前后逐项相同，T2 signed 直证 ARAD_LoginSecurity 0x0818f970 `jg` 保持）。
// S216 旧结论「RHS cast 形仍 LHS-first」成立（4.4 折叠同宽 cast），故必须字段侧双视角。
bool DB_Login::GetPunishUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect punish_type, unix_timestamp(occ_time), punish_value, apply_flag, unix_timestamp(start_time), unix_timestamp(end_time) from member_punish_info where m_id=%s and apply_flag>=1 LIMIT %d",
        NumberToString(sig->m_accId, 0), 0x32);
    // [b28d-solo-DB_Login] P133 合并短路：ORIG `xor$1; test; jne <mov$1 岛>;
    // …get_n_rows…; jne <mov$0>; test; je`（exec 失败与零行汇聚同一返回 1 出口）。
    // P48 no-CSE：三处各自 fresh get_n_rows()（判零 / m_punishInfoCount / 循环上界）。
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return true;   // ORIG 0x8410d7d/0x8410d89：无惩罚数据，正常继续
    }
    unsigned int i = 0;                          // P191：ORIG 双初始化（此处 + for 各一条）
    // [b28d-solo-DB_Login] P211 档③：ORIG `mov %eax,%edx; mov 0xc(%ebp),%eax;
    // mov %edx,0x3a50(%eax)`（RHS-first+中转 3 条）= 原码字段与 get_n_rows() 的 int
    // 返回**异型**（探针 probe_pun f6）。共享头字段 int（同型 ⇒ 4.4 折 LHS-first 2 条）；
    // 视图取址三形（f1/f2/f3）恒 push ebx+sub 4 帧膨胀，局部指针形（f5）多地址槽
    // ⇒ 字段侧旋钮须改共享头，牵动 ARAD_LoginSecurity/InterDispatcher 三处比较符号性，
    // 跨泳道代价 > 本件 1 条 jmp 收益 ⇒ 维持 N18a 封顶（3 行）。
    // [SweepDBLoadEtcMain 本波] 走 SigTypes_D.h 匿名 union 的 unsigned 视角
    // （异型 ⇒ RHS-first+edx 中转），复现 ORIG 08410da9 `mov %eax,%edx;
    // mov 0xc(%ebp),%eax; mov %edx,0x3a50(%eax)`；消费侧保持 int 视角
    // （ARAD_LoginSecurity 0x0818f970 `jg` signed，T2 直证）。探针 /tmp/punp.cpp f↔g。
    sig->m_punishInfoCountU = db->get_n_rows();  // +0x3a50
    for (i = 0; i < db->get_n_rows(); ++i)
    {
        // P157/P183：ORIG 无引用缓存，每次重算完整 i*0x18 地址链。
        if (!db->fetch()) return false;
        if (!db->get_int(0, sig->m_punishInfos[i].m_type)) return false;                // punish_type
        if (!db->get_uint(1, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_field04))) return false;                        // occ_time
        if (!db->get_int(2, sig->m_punishInfos[i].m_stdDropRate)) return false;         // punish_value
        if (!db->get_int(3, sig->m_punishInfos[i].m_field0c)) return false;             // apply_flag
        if (!db->get_uint(4, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_field10))) return false;                        // start_time
        if (!db->get_uint(5, reinterpret_cast<unsigned int&>(
                sig->m_punishInfos[i].m_endTime))) return false;                        // end_time
    }
    return true;
}