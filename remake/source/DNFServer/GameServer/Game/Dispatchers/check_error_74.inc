// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e82fa orig/ours=119/128 ndiff-blocks=16
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%eax | mov    0x11(%eax),%ebx
int Dispatcher_GrowthWeaponEventUseMaterial::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    // [G1Frest 2026-09-10] ORIG 0x081e83xx `mov %eax,-0x14(%ebp)` + 使用点
    //   `mov -0x14(%ebp),%eax; mov 0x11(%eax),%ebx` ⇒ msg 具名指针局部 -0x14，
    //   msg+0x11 在使用点直读（无 slotIdx 具名局部，P0/P183）。
    // [G3 试] disp 折叠（P311/P290）：ORIG 0x081e8347 `mov -0x14(%ebp),%eax;
    //   mov 0x11(%eax),%ebx` 单条常量 disp；旧形 `mbase + 0x11` 产 `add $0x11,%eax;
    //   mov (%eax)`（+1 条）。
    // [G3 2026-09-11] 真 bug 修复：视图缺 packed ⇒ int m_slotIdx 自然对齐到 +0x14，
    //   而 ORIG 0x081e8339 为 `mov 0x11(%eax),%ebx`（+0x11）。同 TU 的
    //   LimitMsgView/BuyItemView 均已用 __attribute__((packed))。
    struct __attribute__((packed)) GrowthWeaponMsgView
    {
        char m_pad11[0x11];    // +0x00..+0x10
        int m_slotIdx;         // +0x11（packed 保证落 +0x11）
    };
    const char* mbase = reinterpret_cast<const char*>(&msg);
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
        1, reinterpret_cast<const GrowthWeaponMsgView*>(mbase)->m_slotIdx);
    if (item->m_addInfo == 0)
    {
        return 0x15;
    }
    if (item->m_addInfo != G_CDataManager()->get_GrowthWeaponMaterialItem())
    {
        return 0x11;
    }
    // [G3 试] 三处 getGrowthWeaponEventdata() 裸偏移（+0x04/+0x10/+0x12）与
    //   G_CEnvironment()+0x378 改具名视图（disp 折叠，P311/P290）。
    struct GrowthWeaponEventView
    {
        int m_field00;             // +0x00
        int m_field04;             // +0x04
        char m_pad08[8];           // +0x08..+0x0f
        unsigned short m_field10;  // +0x10（[P4 负结论 2026-09-11 实测回滚] 改 short 后该件
                                   //   ours 118→121 条 ⇒ gap −1→+2 劣化，虽有符号轴动机仍回滚）
        unsigned short m_field12;  // +0x12
    };
    struct GwpEnvView { char m_pad[0x378]; int m_group; };
    if (reinterpret_cast<GrowthWeaponEventView*>(
            user->getGrowthWeaponEventdata())->m_field04 !=
        user->get_charac_no(-1))                                    // P48 无 CSE：每字段重调
    {
        return 3;
    }
    // ORIG `movl $0x0,-0xc(%ebp)` ⇒ 原码带 `= 0` 初始化
    int serverGroup = 0;
    // [G3 2026-09-11] P59/P222 正向包裹：ORIG 0x081e83db `call IsIntegratedPvPBaseChannel;
    //   test %al,%al; je 081e83f4`（正向、无 xor）⇒ then 必须是 GetServerGroup 侧。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = user->GetServerGroup();
    }
    else
    {
        serverGroup = reinterpret_cast<GwpEnvView*>(G_CEnvironment())->m_group;
    }
    if (static_cast<signed short>(reinterpret_cast<GrowthWeaponEventView*>(
            user->getGrowthWeaponEventdata())->m_field12) !=
        serverGroup)
    {
        return 3;
    }
    // [G3 2026-09-11] 去 signed cast：ORIG 0x081e8432 `movzwl 0x10(%eax),%ebx;
    //   call get_GrowthWeaponMaterialMax; cmp %ax,%bx; setge`（无 movswl）。
    // [Q4DispatchBig 2026-09-11 · 语义轴修复] 实测我方该点 `setae`（unsigned 16 位）
    //   vs ORIG `setge`（signed 16 位）⇒ 原「裸 ushort >= ushort」被 4.4 折成
    //   unsigned 比较；≥0x8000 的材质计数两侧语义不同。ORIG 的 movzwl（零扩展）
    //   说明成员仍是 unsigned short，只把**比较两侧**显式截为 signed short
    //   （同 TU check_error_76 / ChangeInfinity 的 `(short)get_...()` 形已 AE 实证）。
    if (static_cast<signed short>(reinterpret_cast<GrowthWeaponEventView*>(
            user->getGrowthWeaponEventdata())->m_field10) >=
        static_cast<signed short>(G_CDataManager()->get_GrowthWeaponMaterialMax()))
    {
        return 0x11;
    }
    // ORIG 0x081e8469 区：`mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx;
    //   xor $1; test; je` ⇒ 链式虚调（无 event 具名局部）+ `if (!IsEventing(0)) return 1;`
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x64)->IsEventing(0))
    {
        return 1;
    }
    return 0;
}