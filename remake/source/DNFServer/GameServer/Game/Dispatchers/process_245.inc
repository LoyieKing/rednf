int Dispatcher_RequestCharacSkillInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_REQUEST_CHARAC_SKILL_INFO& mref = reinterpret_cast<MSG_REQUEST_CHARAC_SKILL_INFO&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x5b32,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    // ORIG 0x081e3a78/0x081e3aa8/0x081e3ade/0x081e3ba9：四次独立 vector::operator[]
    // （P183 去引用缓存；characIdx 每次从 msg+0xd 重读，P0 表达式直读）。
    // 正极性 if/else（P59/P214①）：ORIG 0x081e3a8a `test %al,%al; je <else>`。
    if (reinterpret_cast<SkillSlot*>(
            reinterpret_cast<CharacInfoFields*>(
                &user->m_characList[(int)(char)mref.m_0d])->m_skill)
            ->IsLoadStrikerSkill())
    {
        // ORIG 0x081e3a8e..0x081e3af7：实参序 kind(call#2)→characIdx→receiver(call#3)；
        // ORIG -0xc 为 kind 具名局部槽（P0 槽往返）。
        int skillTreeKind = (int)(char)
            user->m_characList[(int)(char)mref.m_0d].m_field1239;
        reinterpret_cast<SkillSlot*>(
            reinterpret_cast<CharacInfoFields*>(
                &user->m_characList[(int)(char)mref.m_0d])->m_skill)
            ->SendSkillInfo((int)(char)mref.m_0d,
                            (ENUM_SKILL_TREE_KIND)skillTreeKind);
    }
    else
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x5b40),
                           true);
        // ORIG 0x081e3b3a/0x081e3b62 先调 CStreamGuard::operator*()（P132），
        // 0x081e3b79 调 operator->() 取 GetInBuffer（P103）。
        (*guard)->operator<<(0x2fd);
        (*guard)->operator<<((int)user->GetUID());
        SIG_REQUEST_CHARAC_SKILL_INFO* req =
            guard->GetInBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
        // ORIG 0x081e3ba9 第 4 次 operator[] 后读 +0x00（m_characNo）。
        *(int*)req = (int)user->m_characList[(int)(char)mref.m_0d].m_characNo;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
