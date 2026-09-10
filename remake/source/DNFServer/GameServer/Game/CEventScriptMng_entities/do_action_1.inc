void CAccountFirstLogin::do_action(int flags, CUser* user, std::bitset<32u> /*bs*/)
{
    // ORIG 0x810df04：发首登奖励邮件并请求 DB 落首登标记（仅判 user 空指针）。
    // 服务器组：PvP 统合频道取 GetServerGroup 低字节，否则取 CEnvironment +0x378
    // 低字节（共享头暂无命名字段，用组件视图复现 ORIG 单指令 disp 加载）。
    if (user == 0)
        return;
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail),
        true);

    struct CEnvironment378View
    {
        char         m_pad378[0x378];
        unsigned int m_serverGroup378;   // +0x378
    };
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = (unsigned char)user->GetServerGroup();
    else
        serverGroup = (unsigned char)
            ((CEnvironment378View*)G_CEnvironment())->m_serverGroup378;

    DB_InsertAccountFirstLoginFlag::makeRequest(
        (int)user->GetUID(), user->get_acc_id(), serverGroup,
        (unsigned int)user->getCurCharacNo(), flags);
