// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e39e0 orig/ours=150/119 ndiff-blocks=26
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
// [SweepDispatchers 2026-09-09] 真缺口修复（119→142，ORIG 149；p167 仅剩 C1/C2 同址别名）：
//   ① ORIG 4 次独立 vector<_Charac_info>::operator[]（0x081e3a78/aa8/ade/ba9）——旧码用
//      info/skillSlot 引用缓存只调 1 次 ⇒ 删缓存改表达式直读（P183）；
//   ② ORIG 先调 CStreamGuard::operator*()（0x081e3b3a/0x081e3b62）再 operator<<、operator->()
//      （0x081e3b79）再 GetInBuffer（P132/P103）；旧码直调 guard<< 缺 de/pt 共 3 个 call；
//   ③ 保留 ORIG -0xc 的 kind 具名槽（P0）；正极性 if/else（P59/P214①）。
int Dispatcher_RequestCharacSkillInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G5·§4.3/P95 声明序=槽序] ORIG 序言 `mov 0x14(%ebp),%eax; mov %eax,-0x18(%ebp)`（param
    //   引用槽 -0x18，**先于 msg -0x14 声明**）；各写点单级 disp `mov -0x18(%ebp),%edx;
    //   mov %eax,0x4(%edx)`（0x081e3a07 / 0x081e3a0d / 0x081e3a21 / 0x081e3a2b）。我方裸偏移
    //   `*(int*)((char*)&param+4)` 产 `mov 0x14(%ebp),%eax; add $0x4,%eax` 且无槽。
    stRequestCharacSkillInfo& out = static_cast<stRequestCharacSkillInfo&>(param);
    MSG_REQUEST_CHARAC_SKILL_INFO& mref = reinterpret_cast<MSG_REQUEST_CHARAC_SKILL_INFO&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    // ORIG 0x081e3a07 `mov -0x18(%ebp),%edx; mov %eax,0x4(%edx)`：check_error 结果**直存**
    //   param+4（全文无 err 槽写入）；0x081e3a0d / 0x081e3a21 / 0x081e3a2b 三处均
    //   `mov -0x18(%ebp),%eax; mov 0x4(%eax),%eax` 重读 param+4（含 0x081e3a31 LineFunc
    //   第 3 实参）⇒ 原码无具名 err（P0）；我方 -0x14 err 槽为纯多余。
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x5b32,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
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
}