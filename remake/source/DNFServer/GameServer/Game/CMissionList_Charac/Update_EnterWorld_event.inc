void CMissionList_Charac::Update_EnterWorld_event(CUser& user)
{
    // [b28d-64] P48：ORIG 0x085e600a/0x085e6014 两次独立 `G_GameWorld()->
    // GetChannelType()`（无 CSE，无 type 局部槽）；具名 type 会少 4 条调用且槽漂。
    // [b28d-solo-CMissionList_Charac] P214①：ORIG 0x085e602c/0x085e6037 两守卫
    // 均 `je <mov $0>` 共享落点 + 尾 `mov $1;jmp;test;jne <return>` ⇒ 原码是
    // **正极性早返** `if (A != 0xf && A != 0x10) return;`（负极性包裹被 4.4 反演）。
    // [b28d-solo-CMissionList_Charac 定稿] 原「pvpType/chanType 死局部」判词系
    // **误读**：ORIG 0x085e6080/0x085e6090 两条 store 目标是 param 的成员——
    // param 槽基 -0x23、pack(1) 布局 m_channelFrom@+0x07 → -0x1c、
    // m_channelTo@+0x0b → -0x18（MissionTypes.h 权威偏移），即原码是
    // `param.m_channelFrom = G_CDataManager()->GetPvPChannelType();` +
    // `param.m_channelTo = G_GameWorld()->GetChannelType();`（结果供
    // check_mission_kind 消费，非死代码）。改成员赋值后帧 0x40、out(-0x14)/
    // param(-0x23) 槽位与 ORIG 全等（原 volatile 局部为定位槽序的临时探针，
    // 已删）。
    if (G_GameWorld()->GetChannelType() != 0xf &&
        G_GameWorld()->GetChannelType() != 0x10)
        return;
    alter();
    std::vector<int> out;
    MissionClearCondition_Parameter param(PVP_BATTLE_MODE_2);
    param.m_channelTo = G_CDataManager()->GetPvPChannelType();
    param.m_channelFrom = G_GameWorld()->GetChannelType();
    check_mission_kind(7, user, param, out);
    check_mission_kind(0x1e, user, param, out);
    Send_Cleared_Mission_List(user, out);
