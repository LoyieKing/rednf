// ORIG 0x8418a96：accId==0 直返 1；serverId 取 IsIntegratedPvPBaseChannel ?
// sig+0xe : Server_Envir::m_serverGroup；login_account_%d 表写
// channel_no=+0x8、login_status=0、last_login_date=now()；exec 失败返 0。
// [b28c-L18 判词 N7 寄存器轮换（0xc/0x8 esp 槽值集与 ORIG 全等），L19 代落注释]
// [b28d-solo-4] DIFF→NEAR：P316 volatile 声明 serverId 强制其读先于 m_gcNo
//   （ORIG 4.1 左→右：`mov -0x10,%edx; mov 0x8(%edx),%ecx`；4.4 右→左反序）。
//   现助记符序列与 ORIG 全等（m_gcNo→%ecx、serverId→%edx 角色也一致）；
//   余 serverId 槽 -0x14 vs ORIG -0x10（bool r -0xd vs -0x11）= N82/P112 槽号封顶。
// ORIG 08418a96
bool DB_UpdateLogout::UpdateLogout(SIG_UPDATE_LOGOUT* sig)
{
    if (((DB_UpdateLogoutSigView*)sig)->m_accId == 0)
        return 1;
    // [b28d-solo-4] P316 volatile 旋钮：ORIG 4.1 先装载 serverId（-0x10→%edx）
    // 再算 m_gcNo（→%ecx）；4.4 右→左恒反序。volatile 声明强制该读先行。
    // [L4-r1 判据⑩] 槽序反转律：ORIG 声明序 = bool r → int serverId → ptr db
    //（槽 r@-0x11 / serverId@-0x10 / db@-0xc）。
    // [H4 2026-09-11 复核] 实测把 serverId 提到 r 之前**槽号完全不变**
    //（r 仍 -0xd / serverId 仍 -0x14）⇒ 4.4 按尺寸分组+首次使用序分配、
    // 声明序不可翻（N98/P331 混合尺寸区）⇒ 回滚保原序，判 N82 槽号封顶。
    // [M8DBA r5] 追加一试：`volatile bool r;`（补 volatile 促其提前分配）
    // 读数 58/58 NEAR 不变、槽仍 r@-0xd / serverId@-0x14 ⇒ 无效，回滚。
    bool r;
    // [W8LT-lane 2026-09-11 A/B 定论] 去 volatile 实测 **NEAR→DIFF 劣化**
    //（ndiff 5→2 但助记符序列变，verdict 降档）⇒ 回滚；volatile 是 P316
    // 求值序（ORIG 先 serverId 后 m_gcNo）的必要载体，槽号差（serverId
    // -0x14 vs ORIG -0x10）与 r(-0xd vs -0x11) 维持 N82 槽图封顶。
    volatile int serverId = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverId = ((DB_UpdateLogoutSigView*)sig)->m_serverId;
    else
        serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("upDate login_account_%d set m_channel_no=%d,login_status=%d,last_login_date=now() where m_id=%s",
                  serverId, ((DB_UpdateLogoutSigView*)sig)->m_gcNo, 0,
                  NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}