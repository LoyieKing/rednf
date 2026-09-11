// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d5ca8 orig/ours=89/88 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax
int Dispatcher_WarRoomWpPerMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d5ca8：err 槽 -0x1c 最深（先声明），msg 引用 -0x18、param 引用 -0x14 在其后
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_WARROOM_WP_PER_MONSTER& mref = reinterpret_cast<MSG_WARROOM_WP_PER_MONSTER&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    stWarRoomWp& out = static_cast<stWarRoomWp&>(param);

    // ORIG GetWarRoomIndex → movswl（short）→ CGameManager::GetWarRoom(int)
    // ORIG 0x081d5d00 `movswl %ax,%ebx`（call GetWarRoomIndex 之后直接符号扩展入 %ebx，
    // 跨 G_CGameManager() call 保活，随即 0x081d5d08 `mov %ebx,0x4(%esp)`）：全文无
    // `mov %ax,-0x12(%ebp)` 槽写入 ⇒ 原码无具名 short 局部，值是右值直传（P0/P183）；
    // 我方 `short warRoomIndex` 的多余槽写入/回读即此差异来源。
    WarRoom* warRoom = G_CGameManager()->GetWarRoom((int)user->GetWarRoomIndex());
    if (warRoom == 0)
    {
        return LineFunc(0x3999,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    // ORIG cmp $0xffff; jne <find 块>; movl $0,-0xc; jmp ⇒ if(==0xffff){target=0}else{find}
    CUser* target;
    if (mref.m_killerUid == 0xffff)
    {
        target = 0;
    }
    else
    {
        target = G_GameWorld()->find_from_world(mref.m_killerUid);
        if (target == 0)
        {
            return LineFunc(0x39a4,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
    }

    out.m_4 = warRoom->HandleWpPerMonster(
        user,
        mref.m_field0d,
        mref.m_field11,
        target);
    return 0;
}