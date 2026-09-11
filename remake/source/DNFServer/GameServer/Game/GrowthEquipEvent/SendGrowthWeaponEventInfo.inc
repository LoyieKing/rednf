// ============================================================================
// GrowthEquipEvent::SendGrowthWeaponEventInfo @ 0x080f7226
// 包头 0x1b9：byte 状态（2=礼包已领 / 1=当前角色已选且组一致 / 0=未选）、
// int 已选道具 idx、short +0x10 字段。
// ============================================================================
void GrowthEquipEvent::SendGrowthWeaponEventInfo(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1b9);

    // [b23S3C] ORIG 0x080f7226 形差（本件真语义缺口 gap 5）：
    // ①`movl $0x0,-0xc(%ebp)` 预清 ⇒ serverGroup 写 `= 0` + 分支内独立赋值（P198①两句式）；
    // ②getGrowthWeaponEventdata() ORIG 5 次重取（+0x6a/+0xab/+0xbd/+0x10e/+0x12b），
    //   无 `data` 具名指针槽 ⇒ P48 每用点重取；字段经 GrowthWeaponEvent（CUser.h:1797）
    //   具名访问出单条 disp（原 `*(int*)(data+0xc)` 下标形多 1 条 add，帧亦多 0x10）；
    // ③+0x12/+0x10 为 short 字段：`movzwl 0x12(%eax); movswl %ax,%edx` = 具名 short 成员
    //   与 int 局部比较的直传拓宽形（P200）。
    // [b23S3C] 续：①serverGroup 判定改正向 `if (IsIntegratedPvPBaseChannel())`（ORIG
    // fall-through = GetServerGroup 支，`test %al,%al; je <env>` 无 xor 归一，P191 极性）；
    // ②env 支 ORIG `call G_CEnvironment; mov 0x378(%eax),%eax` 直读字段 ⇒ 改
    //   m_serverEnvir.m_serverGroup（+0x198+0x1e0），不发 get_server_group() call
    // ③selected 去 bool 具名槽（ORIG 结果只占 %eax、
    //   `test %al,%al` 直测）⇒ 内联进 if/else 两 put_byte call。
    // [b28c-L18] 残余 3 复核：`movzwl+movswl %ax,%edx; cmp %eax,%edx` vs 4.4
    // `cwtl; cmp MEM,%eax` = N18a/N22（单段 regmove/内存直比不可翻）；操作数对调
    // （short 字段置左）已试不翻；**Main P230 推论实测负例**：本 TU 追加
    // `-march=i486 -mtune=generic` 重编，regmove/cmp 折叠形原样不动（cmov 族外，
    // 旗标对 lifetime/折叠类差异无效）。
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user.GetServerGroup();
    }
    else
    {
        serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
    }

    if (static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_fieldc != 0)
    {
        packet.put_byte(2);
    }
    else
    {
        if ((user.get_charac_no(-1) ==
             static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field4) &&
            // [b28c-L18] 操作数序对调：ORIG cmp 的 dst=短字段拓宽值（`movswl %ax,%edx`
            // 后 `cmp %eax,%edx`），即 short 字段为左操作数。
            (static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field12 ==
             serverGroup))
        {
            packet.put_byte(1);
        }
        else
        {
            packet.put_byte(0);
        }
    }
    packet.put_int(
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field8);
    packet.put_short(
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field10);
    packet.finalize(true);
    user.Send(packet);
}