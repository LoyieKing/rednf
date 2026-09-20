// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d73f6 orig/ours=64/64 ndiff-blocks=14
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x18(%ebp)
int Dispatcher_ChangeHostWarRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [L7DispG4 2026-09-11 · P0/P183/P-L11c] ORIG 0x081d73f6 槽集直证：
    //   err→-0x18、msg→-0x14、param→-0x10、room→-0xc（4 个具名），
    //   **无** roomIdx/mgr/result 槽：
    //   ① 0x081d744e `call GetWarRoomIndex; movswl %ax,%ebx`（结果 %ebx 直传实参，
    //      求值在 receiver 之前）⇒ 内联进 GetWarRoom 实参；
    //   ② 0x081d7451 `call G_CGameManager` 结果直用 %eax ⇒ 链式 receiver；
    //   ③ 0x081d74c2 `call HostChangeWarRoom; mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)`
    //      ⇒ 返回值直写 param+0x4，无 result 槽。
    //   msg/param 两个具名指针槽在两条 guard 之后才发射（P-L11c 声明位置律）。
    int err;
    err = check_error(user, msg);
    if (err > 0)
        return err;
    if (err < 0)
        return -1;
    MSG_BASE* pm = &msg;
    ParamBase* pp = &param;
    WarRoom* room = G_CGameManager()->GetWarRoom((short)user->GetWarRoomIndex());
    if (room == 0)
    {
        return LineFunc(0x41a7,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    else
    {
        // [L7DispG4 2026-09-11 · 技巧14/32] ORIG 0x081d749e/0x081d74a1 为**单级 disp 读**
        //   （`mov 0xe(%eax),%edx` / `movzbl 0xd(%eax),%eax`，基址来自 msg 槽回读）
        //   ⇒ 用 packed 视图成员承载，禁裸 char*+disp（4.4 会退化成 add+mov 两步）。
        // [P4 负结论 2026-09-11 实测回滚 V1] 「裸指针 + 常量偏移」形实测**劣化**：
        //   复编后 ours 63→65（gap 0→−2，多 2 条），blocks 仍 2 ⇒ 4.4 仍提前物化基址并
        //   额外发 lea；既有 12112-12114 负结论（`(char*)pp + 0x4` 被缓存成
        //   `lea 0x4(%eax),%ebx`）在本件再次证实。维持「视图成员」形（blocks=2，N7 封顶）。
        static_cast<stChangeHostWarRoom*>(pp)->m_4 =
            room->HostChangeWarRoom(
                user,
                reinterpret_cast<MSG_CHANGE_HOST_WARROOM*>(pm)->m_field0d,
                reinterpret_cast<MSG_CHANGE_HOST_WARROOM*>(pm)->m_field0e);
    }
    return 0;
}