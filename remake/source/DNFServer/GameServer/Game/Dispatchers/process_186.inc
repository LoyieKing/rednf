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
        // ORIG 0x081cfa29: `mov -0x18(%ebp),%eax; movl $0x15,0x4(%eax)` —— 写回 out 引用
        //（原 `*(int*)((char*)&param+4)=…` 会产 `mov 0x14(%ebp),%eax; add $0x4,%eax` 形）。
        out.m_4 = 0x15;
        return 0;
    }
    // ORIG 0x081cfa3d: `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— 具名 msg 视图指针占槽
    //（原内联 `reinterpret_cast<...>(&msg)->…` 无槽，ORIG 每次经 -0x10 重载）。
    const DisjointStoreMsgView* view = reinterpret_cast<const DisjointStoreMsgView*>(&msg);
    user->SetCurCharacExpertJob(job);
    // ORIG 0x081cfa55: `movl $0x0,-0xc(%ebp)` —— result 具名量调用前 0 预清。
    int result = 0;
    result = reinterpret_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        user,
        view->m_storeName,
        view->m_field114,
        (int)view->m_field118,
        (int)view->m_field11a);
    if (result != 0)
    {
        user->SetCurCharacExpertJob(0);
        GlobalData::s_ExpertJobMgr->FreeExpertJob(job);
        // ORIG 0x081cfadc: `mov -0xc(%ebp),%edx; mov -0x18(%ebp),%eax; mov %edx,0x4(%eax)`
        out.m_4 = result;
    }
    return 0;
