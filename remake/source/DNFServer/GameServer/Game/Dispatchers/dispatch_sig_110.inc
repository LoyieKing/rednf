int Dispatcher_CompleteLoadAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // [FL1 2026-09-10] ORIG 0x08206fXX 直证：state<=1 支的 `call LineFunc(0xa199)`
    //   之后 `jmp <函数尾共享 return 0>`（无就地 `mov $0x0,%eax`）⇒ 原码是
    //   if/else 单一尾 return 结构（旧形每支各自 `return 0` 使 4.4 就地物化 +2 条）。
    if (user->get_state() <= 1)
    {
        LineFunc(0xa199,
                 __PRETTY_FUNCTION__,
                 0, 0);
    }
    else
    {
        PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
        if (room != 0 && room->m_matching != 0)
        {
            ((IMatch*)room->m_matching)->OnCompleteLoad(user);
        }
        else
        {
            // ORIG 0x0820..：[40] 段直接 `mov 0xc(%ebp),%edx; mov %edx,0x4(%esp)`
            //   （无 mgr 具名指针槽；我方具名使 room 槽从 -0xc 下移到 -0x10）。
            if (!pvp_assault::GetInstanceAssaultMgr()->OnCompleteLoad(user))
            {
                LineFunc(0xa1a8,
                         __PRETTY_FUNCTION__,
                         0, 0);
            }
        }
    }
    return 0;
