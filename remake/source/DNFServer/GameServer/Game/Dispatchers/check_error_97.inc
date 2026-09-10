unsigned int Dispatcher_GuildCargoPopItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    // [G1Frest 2026-09-10] ORIG 0x081d14aa 全形重写（逐条对照 0x081d14aa..0x081d15b1）：
    //   ① 提前返回结构 + 各条件按 ORIG 极性/bool 物化（diamond `mov $1/%mov $0`、
    //      `sete`、`xor $1`），旧码为嵌套 if + guildkey 具名局部（多 -0x18 槽、帧
    //      0x28 vs ORIG `push %ebx; sub $0x24`）；
    //   ② 0x081d154e `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— itemIdx 视图
    //      取自 **msg**（第 2 参；0xc=user/0x10=msg/0x14=param），旧码误用 param
    //      ⇒ **参数错位真 bug**。
    (void)param;

    if ((bool)(user->get_state() != 3 || user->getCurCharacR() == 0))   // 0x081d14b7 起 diamond
        return -1;
    if ((bool)(user->get_charac_guildkey() == 0))                       // 0x081d14f0 `sete` 岛
        return 100;
    if (!user->IsExistGuildAgit())                                      // 0x081d150e `xor $1`
        return 0xb5;

    int secu = GlobalData::s_pSecuProtectionField->Check(               // ORIG -0x14 具名
        user, (SECURITY_PROTCTION)9);                                   // 0x08288a02
    if (secu != 0)
        return secu;

    // msg +0x0f（packed 具名视图，ORIG 单级 disp `mov 0xf(%eax),%ebx`）
    struct __attribute__((packed)) CargoPopParamView {
        char m_head[0xf];
        int m_itemIdx;                   // +0x0f
    };
    CargoPopParamView* pv = reinterpret_cast<CargoPopParamView*>(&msg);  // ORIG -0x10
    const CInventory* inven = user->getCurCharacInvenR();
    // ORIG 0x0850562a 之后 `test %eax,%eax; jne` 直分支（int callee 免物化）⇒ 写 == 0。
    if (inven->possible_to_insert(pv->m_itemIdx) == 0)
        return 0xcb;
    // ORIG 0x081d1584 区：`mov -0x10(%ebp),%eax; mov 0xf(%eax),%ebx` 二次重读（无缓存）
    if (G_CDataManager()->find_item(pv->m_itemIdx) == 0)                 // ORIG 0x0835fa32
        return 0xc5;
    return 0;
