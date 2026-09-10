int DisPatcher_CreateCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stCreateCharac& out = static_cast<stCreateCharac&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x79c,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }

    G_CDataManager()->setCharacNameFilterPass(
        GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ? true : false);

    // [G5-ProcB] ORIG 0x081c04d4 `mov 0x10(%ebp),%eax; mov %eax,-0x24(%ebp)`：
    // msg 视图具名局部在 setCharacNameFilterPass 之后初始化（函数头声明会多一条
    // store 并把槽基整体下移）。
    MSG_CREATE_CHARACTER& req = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);

    // [G5-ProcB] ORIG 0x081c04ef Acquire → 0x081c04f4 `movl $0x1,0x8(%esp)` /
    // 0x081c04fc `mov %eax,0x4(%esp)` / 0x081c0500 `lea -0x30(%ebp),%eax;(%esp)`：
    // Acquire 返回值经 %eax 直传 CStreamGuard 构造实参，ORIG 无 `stream` 具名槽。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x7ab), true);
    // ORIG 0x081c0511/0x081c0539 每次 << 先 operator*()（de 0x080c8c46）再 operator<<
    guard.operator*()->operator<<(4);             // SIG 类型 4
    guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    SIG_CREATE_CHARAC* sig = guard.operator->()->GetInBuffer<SIG_CREATE_CHARAC>();
    memset(sig, 0, 0x5354);
    sig->m_accId = user->get_acc_id();
    sig->m_slot = req.m_slot;
    strncpy(sig->m_name, req.m_name, 0x1e);
    // [DPCB 2026-09-11] 同型视图轴：源 `char req.m_name[0x1d]`(+0x2f) 与目标
    // `unsigned char sig->m_field23` **异型** ⇒ 4.4 插值转换临时
    // （`movzbl 0x2f(%eax),%eax; mov %eax,%edx`），ORIG 单条
    // `mov -0x24(%ebp),%eax; movzbl 0x2f(%eax),%edx`。改 pack(1) 同址视图读成
    // unsigned char（同型 ⇒ 无转换临时），偏移/位型不变。
    #pragma pack(push, 1)
    struct CreateCharacReqView {
        char m_head[0x2f];
        unsigned char m_byte2f;      // +0x2f ≡ m_name[0x1d]
    };
    #pragma pack(pop)
    sig->m_field23 =
        reinterpret_cast<const CreateCharacReqView*>(&req)->m_byte2f;
    if (sig->m_field23 != 0 && user->getUserEventCharacterFlag() != 0)
    {
        out.m_errNo = 0x14;                       // 活动角色创建位占用
        return 0;
    }

    sig->m_eventCharacFlag = user->getUserEventCharacterFlag();
    // 角色创建表：G_CDataManager()->m_characList 基址 + job*0x7dc
    // （与 SkillSlot.cpp:43 / CUser.cpp:4820 同口径；CCharacList 权威布局未建模）
    // P183 地址重物化：ORIG 0x081c0614 取一次基址供 m_field26+memcpy（`add $0x2` 复用），
    // 0x081c064c 在 m_field78 处**重新** call G_CDataManager() 并重算 imul 0x7dc 全链
    // （我方原先两处共用 jobRec ⇒ p167 MISSING _Z14G_CDataManagerv ×1）。
    const unsigned char* jobRec =
        reinterpret_cast<const unsigned char*>(G_CDataManager()->m_characList) +
        // [G5-ProcB] ORIG 0x081c061f/0x081c0657 均 `mov -0x24(%ebp),%eax;
        // movzbl 0xd(%eax),%eax; movsbl %al,%edx` ⇒ 本处 slot 取自 **msg**+0xd
        // （req.m_slot，signed char 语义两段扩展），非 sig+0x22。
        static_cast<int>(static_cast<char>(req.m_slot)) * 0x7dc;
    sig->m_field26 = *reinterpret_cast<const unsigned short*>(jobRec);
    // [J5-ProcD 2026-09-11 核账] ORIG 0x081c0641 `mov $0x14,%ecx; mov %eax,%edi;
    // mov %edx,%esi; rep movsl`（=20 dword/80B 的 gcc **movmem 内联**，非 call memcpy）
    // + 函数头 0x081c0301 `push %edi` 同证；p167 差集：我方多 1 个 `memcpy` 调用
    // （ORIG calls=38 / 我方 39）。
    // 两次试形均已实测劣化 + 回滚：①H5 `struct JobDataBlob{...} = *(...)`（具名 80B
    // struct）271→276(+5)；②本轮**纯 dword 数组视图**（20×unsigned int）272→276(+4)
    // —— 4.4 -O0 对 80B 一律走 by-pieces（movl 序列）而非 movmem，**勿再试结构体整体
    // 赋值/prism 视图**；p167 的 EXTRA memcpy 记 N（需 movmem 之外的旋钮）。
    memcpy(sig->m_jobData, jobRec + 2, sizeof(sig->m_jobData));
    sig->m_field78 =
        (reinterpret_cast<const unsigned char*>(G_CDataManager()->m_characList) +
         static_cast<int>(static_cast<char>(req.m_slot)) * 0x7dc)[0x7d4];
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillA),
        // [G5-ProcB] ORIG 0x081c0695 `mov -0x20(%ebp),%eax; movzbl 0x22(%eax),%eax;
        // movsbl %al,%eax` ⇒ 此处才是 sig+0x22（符号扩展两段）。
        static_cast<int>(static_cast<char>(sig->m_slot)));
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillB),
        static_cast<int>(static_cast<char>(sig->m_slot)));
    sig->m_spAtLevelUp = G_CDataManager()->GetSpAtLevelUp(1);
    makeGift(user, sig);

    const char* ssn = user->GetSsnString();
    // [DPC_Process] ORIG 0x081c0xxx 段：`cmp $0xc,%eax; jbe <写1>`、
    // `cmp $0x32,%al; je <写0>`、`cmp $0x34,%al; jne <写1>` + 两支各
    // `movb $0x0/$0x1,0x534c(%eax)`（**立即数直写，无 %al 中转**）⇒ 原码为
    // if/else 直赋值；旧文三元表达式使 4.4 产 `mov $0x1/%0x0,%eax; mov %al,槽`。
    if (strlen(ssn) < 0xd || (ssn[6] != '2' && ssn[6] != '4'))
    {
        sig->m_ssnCheckFail = 1;
    }
    else
    {
        sig->m_ssnCheckFail = 0;
    }

    if (sig->m_slot == 9)
    {
        user->setComboSkillSet(true);
    }
    if (!user->lockCreateCharac())
    {
        return LineFunc(0x81f,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
