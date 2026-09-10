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
    sig->m_field23 = static_cast<unsigned char>(req.m_name[0x1d]);
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
    // N84 实测反例：结构体整体赋值在 4.4.7 下退化为逐字节序列（ours 273→310、ndiff 112→118）
    // ⇒ 保留 memcpy 调用（ORIG 0x081c0641 的 rep movsl 属 N78，不可翻）。
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
    sig->m_ssnCheckFail =
        (strlen(ssn) < 0xd || (ssn[6] != '2' && ssn[6] != '4')) ? 1 : 0;

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
