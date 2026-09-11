// [b28d-solo-W6_AssaultMgrDeps] 封顶 N85（253/257 insn）：已修 GetAutoClientRate 取 target（真 bug）、
// 去 event 判空 / powerWarOn 局部 / chaosCapSec、ROUND 改 static_cast<int> 截断内联、
// CDataManager+0x600/604/608 具名视图（+0x608 为 float）。残余 = 4 处 x87 比较
// lowering（ORIG fucompp/fnstsw/test$5,ah/sete 各多 1 条）+ x87 求值序临时槽
// （-0x50/-0x4c/-0x48 vs -0x40 区；具名 float 临时探针反致浅槽漂移 0x0c）。
// [S2W6Deps 复核·负结论] gap=+4 归因确认 = **编译器中端固有差，源码不可造**：
// ORIG（原厂旧 gcc）每处浮点比较发 4 条 `fucompp; fnstsw %ax; test $0x45,%ah;
// sete %al`（5 处 ×4=20），我方 gcc11 发 3 条 `fucomip %st(1),%st; fstp %st(0);
// setae/seta %al`（5 处 ×3=15）⇒ 净 +4~+5。探针 /tmp/fpcmp/t.cpp（gcc11 -m32 -O0
// 编译 `a<=0.0f / a>=b / a>b / a-b<=0.0f` 四式）实测恒出 `fcomip/fucomip+setcc`，
// 不因源码改写（`>=`↔`<=` 翻转、具名 float 临时、`(double)`/`(float)` 混型）而变；
// 与 N97「有符号除法 lowering cmovs」同族。已由父泳道 N3Free171to190 复核接受。
// ORIG 0x082edc86。
int CAssaultMgr::GetRequestAssaultNeedMoney(CUser* attacker, CUser* target)
{
    // ORIG 无 event 具名槽（P0：指针留 %eax 直传虚调）+ 无判空（P171 照抄）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing(0))
    {
        return 0;
    }

    // ORIG 无具名 powerWarOn 局部（P0：&& 短路值仅存寄存器）。
    if (PowerWarOn() && attacker->getCurCharacVill() == 0x7)
    {
        return 0;
    }

    const int basePrice =
        G_CDataManager()->m_serverParameter.getAssaultPrice(
            attacker->get_charac_level());

    // 目标混沌点==0：主价格公式分支。
    if (target->GetCurCharacChaosPoint() == 0)
    {
        // ORIG 取 target 的 AutoClientRate（0x082edd3a `mov 0x10(%ebp),%eax`）。
        const float autoClientRate =
            static_cast<float>(target->GetAutoClientRate()) / (-100.0f) + 1.0f;
                                                            // _DAT_08c1d18c=-100.0f
        int one = 1;                                        // ORIG -0x24 `movl $0x1`
        float decaySec = static_cast<float>(
            GlobalData::s_systemTime_.getCurSec()
            - attacker->GetCurCharacChaosKillTime());
        if (decaySec <= 0.0f)
        {
            decaySec += 1.0f;
        }
        else
        {
            if (ROUND_NEAREST_EVEN(decaySec) > DM_FIELD(604))
            {
                decaySec = static_cast<float>(DM_FIELD(604));
            }
        }
        float upperBound =
            static_cast<float>(DM_FIELD(604)) * autoClientRate;
        if (upperBound <= 0.0f)
        {
            upperBound += 1.0f;
        }
        float factor = 1.0f;
        if (decaySec - upperBound <= 0.0f)
        {
            factor = (static_cast<float>(DM_FIELD(600))
                      - decaySec / upperBound) * one;
        }
        if (factor >= 1.0f)
        {
            if (factor > static_cast<float>(DM_FIELD(600)))
            {
                factor = static_cast<float>(DM_FIELD(600));
            }
        }
        else
        {
            factor = 1.0f;
        }

        // 等级差惩罚（CDataManager+0x608 系数）。
        const int levelDiff =
            attacker->get_charac_level() - target->get_charac_level();
        float levelPenalty = 1.0f;
        if (levelDiff > 8)
        {
            levelPenalty = DM_FIELD(608)
                               * static_cast<float>(levelDiff - 8) + 1.0f;
        }

        const float priced = static_cast<float>(ROUND_NEAREST_EVEN(
            static_cast<float>(basePrice) * factor * levelPenalty * autoClientRate));
        return ROUND_NEAREST_EVEN(
            static_cast<float>(
                G_CDataManager()->m_serverParameter
                    .getAssaultPriceChaospointWeight(
                        attacker->GetCurCharacChaosPoint()))
            * priced);
    }

    // 目标混沌点!=0：只按目标混沌权重缩放基础价。
    const float priceF = static_cast<float>(basePrice);
    return ROUND_NEAREST_EVEN(
        G_CDataManager()->m_serverParameter.getChaosPointWeight(
            target->GetCurCharacChaosPoint())
        * priceF);
}