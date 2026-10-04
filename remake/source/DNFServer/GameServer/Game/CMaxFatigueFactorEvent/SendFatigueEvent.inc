// ---------------------------------------------------------------------------
// ORIG 0x0812ca88（CLocalChina_User::SendFatigueEvent，报告
// CLocalChina_User/SendFatigueEvent.md）：
// GetRepeatEvent(2) → do-while 遍历 m_UserInWorld（循环头 G_GameWorld()+
// end()+!= 判定，不符即 return）→ state>2 才处理：ev 存在时经 getCurCharacW
// 写 ch->m_maxFatigue（IsEventing(0) ? factor*DEFAULT_MAX_FATIGUE/100 :
// DEFAULT_MAX_FATIGUE；ch==0 跳过 premium/SendFatigue 直入 ++it）；
// IsHavePremiumAdvantage 时按事件是否生效分两路 setCurCharacMaxPremiumFatigue
// （生效：事件缩放差 + 优惠倍率两段 /100；未生效：当前上限 × 优惠倍率 /100）；
// 最后 CUser::SendFatigue()。
// [N6S1] NEAR | ORIG 0x0812ca88 | P13/P28 已修（while 形 + `it++` 复现
// `jmp cond` + 尾部 `ppEi`）；[N6S1] 本轮再落 P0/P183：删 `bool premiumEventing`
// 具名槽（改条件位 `ev != 0 && ev->IsEventing(0)`）+ 删 rate1/iVar3/uVar6/rate2/
// curMax/rate 具名中间量（改内联表达式 + 提升型乘法，见其处注释）⇒ 帧 0x54→0x30、
// 229→215 条 = ORIG 216/215（pushes/leave→`lea -0x8(%ebp),%esp` 尾岛同步复现）。
// 残差（封顶）：两处虚调用我方的 fn/this 寄存器为 ORIG 的镜像（证据见其处注释）。
void CLocalChina_User::SendFatigueEvent()
{
    CMaxFatigueFactorEvent* ev =
        static_cast<CMaxFatigueFactorEvent*>(GlobalData::s_event_manager->GetRepeatEvent(2));
    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();
    // [b28d-28] ORIG 0x0812cac5 `jmp <cond>`（while 前置跳转）+ 条件处每轮
    // `call end()`（非缓存）+ 尾部 `ppEi` 后置增量 ⇒ 源形 while(it != end())。
    while (it != G_GameWorld()->m_UserInWorld.end())
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            if (ev != 0)
            {
                _Charac_info* ch = user->getCurCharacW();
                if (ch != 0)
                {
                    // [N6S1 封顶·证据] ORIG 0x0812cb17 `add $0x34,%eax` →
                    // 0x0812cb1a `mov (%eax),%edx`（fn→edx）→ 0x0812cb1c
                    // `mov -0x18(%ebp),%eax`（this→eax）→ 0x0812cb2a `call *%edx`。
                    // 4.4.6 对同一源形恒产镜像 fn→eax / this→%edx（本条与
                    // 0x0812cb8f 处第二个 IsEventing 调用点皆然），指令数/助记符
                    // 序列全同 ⇒ 属 ORIG 编译器版本寄存器分配差，非源形可达；
                    // 试过 P 类旋钮无法翻转，本函数封顶 NEAR。
                    if (ev->IsEventing(0))
                    {
                        ch->m_maxFatigue = static_cast<unsigned short>(
                                                        ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100);
                    }
                    else
                    {
                        ch->m_maxFatigue =
                            static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
                    }
                }
                else
                {
                    goto loop_tail;   // ORIG ch==0 跳过 premium/SendFatigue 直入 ++it
                }
            }
            if (user->IsHavePremiumAdvantage())
            {
                // [ndiff] ORIG 0x0812cb89：`cmpl $0x0,-0x18(%ebp); je`（ev==0 先判）
                // → `call *0x34` → `mov $0x1,%eax / jmp / mov $0x0,%eax / test %al`
                // = 条件位 `(ev != 0) && ev->IsEventing(0)` 的短路形（eax 承载，
                // 无 bool 具名槽）。原先的 if/else 赋值 bool 会多出 -0x23 槽的
                // movb $0/$1/cmpb 三条。
                if (ev != 0 && ev->IsEventing(0))
                {
                    // ORIG 0x0812cbc1..0x0812cc1d：仅 local_e 占 -0xa 槽；
                    // 其余中间量全在 ebx/esi 跨调用保活（t1→ebx，local_e→esi，
                    // P0/P183：ORIG 无 rate1/iVar3/uVar6/rate2 槽）。
                    unsigned short local_e = static_cast<unsigned short>(
                                                static_cast<short>(ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100) -
                        static_cast<short>(DEFAULT_MAX_FATIGUE));
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<short>(static_cast<int>(
                            static_cast<unsigned short>(
                                user->GetPremiumInfo()->GetAdvantageFatigueRate()) *
                            DEFAULT_MAX_FATIGUE) / 100) +
                        static_cast<short>(static_cast<int>(
                            static_cast<unsigned short>(local_e) *
                            static_cast<unsigned short>(
                                user->GetPremiumInfo()->GetAdvantageFatigueRate())) / 100)));
                }
                else
                {
                    // ORIG 0x0812cc86..0x0812ccd3：`movzwl %ax,%ebx` 后跨两次调用
                    // 保活，无 curMax/rate 槽（P0/P183）。
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<int>(
                            static_cast<unsigned short>(
                                user->getCurCharacMaxFatigue()) *
                            static_cast<unsigned short>(
                                user->GetPremiumInfo()->GetAdvantageFatigueRate())) / 100));
                }
            }
            user->SendFatigue();
        }
loop_tail:
        // [b28d-28] P28 增量侧：ORIG 增量段调 `_Rb_tree_iterator::ppEi`（后置，
        // 带独立 sret 临时槽）⇒ 源形必写 `it++`；`++it` 走 ppEv 免临时槽。
        it++;
    }
}