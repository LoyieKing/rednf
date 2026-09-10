int Dispatcher_CreateDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCreateDisjointStoreParam& out = static_cast<stCreateDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x29c4,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    // MSG_CREATE_DISJOINT_STORE 冻结头未建模 payload（仅 m_pad[0x100]）；
    // §8.3 TU 局部视图具名（偏移/宽度证据=本函数 ORIG + read get_* 序列）。
    struct DisjointStoreMsgView    // 自然对齐恰落各偏移（0xd+0x103=0x110 已 4 对齐）
    {
        char m_head[0xd];
        char m_storeName[0x103];   // +0x0d（get_str max 0x100）
        int m_field110;            // +0x110 名字长度（get_int）
        int m_field114;            // +0x114
        short m_field118;          // +0x118
        short m_field11a;          // +0x11a
    };
    expert_job::CExpertJob* job = GlobalData::s_ExpertJobMgr->AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)3);
    if (job == 0)
    {
        *(int*)((char*)&param + 4) = 0x15;
        return 0;
    }
    user->SetCurCharacExpertJob(job);
    int result = reinterpret_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        user,
        // 内联视图（ORIG 本函数无 msg 指针槽，禁新增局部保帧零扰动）
        reinterpret_cast<DisjointStoreMsgView*>(&msg)->m_storeName,
        reinterpret_cast<DisjointStoreMsgView*>(&msg)->m_field114,
        (int)reinterpret_cast<DisjointStoreMsgView*>(&msg)->m_field118,
        (int)reinterpret_cast<DisjointStoreMsgView*>(&msg)->m_field11a);
    if (result != 0)
    {
        user->SetCurCharacExpertJob(0);
        GlobalData::s_ExpertJobMgr->FreeExpertJob(job);
        *(int*)((char*)&param + 4) = result;
    }
    return 0;
