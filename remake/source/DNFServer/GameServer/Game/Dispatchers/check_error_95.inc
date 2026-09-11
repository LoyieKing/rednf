// ==== Dispatcher_SoloTeleport::check_error (ORIG 0x081e1e98) ====
// 注意:Dispatchers.h 现缺本声明,接入时须在类内补(见文件头)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e1e98 orig/ours=107/111 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x20(%ebp) | mov    -0x20(%ebp),%eax | mov    -0x20(%ebp),%eax
int Dispatcher_SoloTeleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081e1e98：result 槽(-0x1c)在 && 判定之后才预清 0（P198①位置）；
    // 无 tp 缓存局部——每用点重取 `mov -0x20,%eax; mov 0x44(%eax),%eax`（P48，ORIG
    // 5 次 get_party_teleport call 实证）；dir/posY 前两 getter 落 int 槽(-0x30/-0x2c)，
    // posx/area/village 实参直传升格 edi/esi/ebx（P200，右至左求值序 dir→village）；
    // vcall 用虚槽形去 fn 中间槽。末参 2=强制通过同村检查。
    (void)param;
    if (user == 0)
    {
        return -1;
    }
    MSG_SOLO_TELEPORT& req = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    if (req.m_30 == -1 && req.m_34 == -1)
    {
        return -1;
    }
    int result = 0;
    STCheckRuleData rule;
    rule.m_field00 = req.m_30;
    rule.m_user = user;
    result = static_cast<Teleport*>(*reinterpret_cast<void**>(req.m_44))
        ->check_rule(rule);
    if (result == 0)
    {
        int dir = static_cast<Teleport*>(*reinterpret_cast<void**>(req.m_44))
            ->get_party_teleport().get_teleport_direction();
        int posY = static_cast<Teleport*>(*reinterpret_cast<void**>(req.m_44))
            ->get_party_teleport().get_teleport_posy();
        result = G_GameWorld()->check_move_area(
            user,
            static_cast<int>(static_cast<Teleport*>(
                *reinterpret_cast<void**>(req.m_44))
                ->get_party_teleport().get_teleport_village()),
            static_cast<int>(static_cast<Teleport*>(
                *reinterpret_cast<void**>(req.m_44))
                ->get_party_teleport().get_teleport_area_index()),
            static_cast<int>(static_cast<Teleport*>(
                *reinterpret_cast<void**>(req.m_44))
                ->get_party_teleport().get_teleport_posx()),
            posY,
            dir,
            true, 2);
    }
    return result;
}