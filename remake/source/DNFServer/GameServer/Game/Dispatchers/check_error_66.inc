// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081db580 orig/ours=48/43 ndiff-blocks=5
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x10,%esp
int Dispatcher_Select_Item_Grwoth_Power::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081db580：state>2 且当前角色非空才放行；随后要求
    // (growth_power_reward & 0xff) < 当前段下界（get_break_away_section_low_level）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    // [G1Frest 2026-09-10] ORIG 0x081db580 区：reward 落 %esi（`movzbl %al,%esi`）、
    //   level 落 %ebx，随后 `call G_CDataManager; add $0x8710,%eax` 取 &m_growthPower，
    //   `cmp %eax,%esi; setge` ⇒ **无 level/low/reward 具名局部**
    //   （旧码 -0x14/-0x10/-0x9 三槽为发明；P0/P183 全表达式化）。
    // 极性：ORIG 0x081db5f1 `setge %al; test %al,%al; je <返回 0 岛>` —— 正向 `>=`
    //   先测、真分支落 `mov $0xffffffff`；改用 `>=` 正向使两块布局与 ORIG 一致。
    if (static_cast<int>(user->get_growth_power_reward()) >=
        G_CDataManager()->m_growthPower.get_break_away_section_low_level(
            user->get_charac_level()))
    {
        return -1;
    }
    return 0;
}