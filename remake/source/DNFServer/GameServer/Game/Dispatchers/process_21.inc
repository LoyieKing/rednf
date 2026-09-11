// err>0 → 0；err<0 → LineFunc(0x33f)；否则当前角色 SkillSlot 写入组合技映射
// 并校验该树。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825f2fc orig/ours=64/62 ndiff-blocks=13
//   帧差 sub $20 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x20,%esp
int Dispatcher_ComboSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    out.m_result = check_error(user, msg);

    if (out.m_result > 0)
    {
        return 0;
    }
    if (out.m_result < 0)
    {
        return LineFunc(0x33f,
                        __PRETTY_FUNCTION__,
                        out.m_result, 0);
    }
    // ORIG 0x0825f385 `call getCurCharacSkillW` → 0x0825f39b `call setComboSkillInfo`
    // （结果 %eax 直作 this，0x0825f3b0/0x0825f3bc 第二处同形）：**无**具名槽
    // ⇒ 删 skillW 具名缓存（原 -0xc 每处一对 store/load）⇒ P0/P183。
    MSG_COMBO_SKILL& req = reinterpret_cast<MSG_COMBO_SKILL&>(msg);
    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())

        ->setComboSkillInfo((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d,
                            req.m_comboMap);
    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
        ->verifyComboSkillTree((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d);
    return 0;
}