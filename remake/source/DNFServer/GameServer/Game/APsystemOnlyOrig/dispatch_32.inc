// ORIG 0x841826a：GetOutBuffer<SIG_UPDATE_LOGIN> → 表号取集成 PvP 通道 ?
// sig+0x08 : CEnvironment+0x378（m_serverEnvir.m_serverGroup）→ GetDBHandle
// (6,0) → upDate login_account_%d（m_channel_no=+4 / login_status=1 / now() /
// login_ip=+0x0c / m_id=NumberToString(+0)）；exec 失败或 affected==0 补
// inSert 新行（inSert exec 失败返 0）；全绿返 1。
// [b28d-4] NEAR：NEAR 封顶：助记符序列全同、仅栈槽地址/寄存器文本差（N82 槽号错位 / N7 寄存器轮换）；ORIG 0841826a
// [b28d-solo-4] NEAR 复核：残差 = tableNo 装载寄存器 %eax vs ORIG %edx（两段 set_query
//   各 1 处）+ 首段 login_ip/channelNo 的 %ecx/%ebx 角色互换（N7 寄存器轮换，446 后端下
//   仍存；第二段 esi/ebx 已与 ORIG 一致）⇒ N7/N82 封顶。
bool DB_UpdateLogin::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_UPDATE_LOGIN* sig = stream->GetOutBuffer<SIG_UPDATE_LOGIN>();
    if (sig == 0)
        return 0;
    int tableNo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        tableNo = ((DB_UpdateLoginView*)sig)->m_serverGroup;
    else
        tableNo = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate login_account_%d set m_channel_no=%d,login_status=1,last_login_date=now(),login_ip='%s' where m_id=%s",
        tableNo, ((DB_UpdateLoginView*)sig)->m_channelNo,
        ((DB_UpdateLoginView*)sig)->m_loginIp,
        NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into login_account_%d (m_id,m_channel_no,login_status,last_login_date,login_ip) values(%s,%d,1,now(),'%s')",
            tableNo, NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0),
            ((DB_UpdateLoginView*)sig)->m_channelNo,
            ((DB_UpdateLoginView*)sig)->m_loginIp);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}