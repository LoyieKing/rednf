// ---- ORIG T 0x0813101e（advancealtar__Manager/eplp.md 全文）----
// [b28d-solo-advancealtar_Manager N1 封顶] 第三守卫正向 if（P257）与出口岛/块序
// 逐条一致；唯余 ORIG 0x08131041 `add $0x0,%eax`（4.1 对 `type+EPLP_N0` 未折叠，
// 4.4 前端恒折；探针 /tmp/eplp_probe{,2}.cpp 七形 + 本次 /tmp/b28d_altar_probe/
// eplp3.cpp 五形（type+EPLP_N0 / +0u / (uint)cast / (int)+0 / uint+EPLP_N0）
// 全折，4.4.6 后端亦同）⇒ N1。实测 24 vs 23 条（仅差该 add），语义等价。
void Manager::eplp(CUser* user, EplpType::T type)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    // [b28d-solo-advancealtar_Manager] ORIG +0x29 `ja +0x3c`(leave) 无独立返回岛 ⇒
    // 第三守卫是正向 if 包裹 body（P257），非 early-return。
    if (static_cast<unsigned int>(type) <= static_cast<unsigned int>(EplpType::EPLP_N2))
    {
        control->leaveUser();
    }
}