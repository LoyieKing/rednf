// ===========================================================================
// 7) HeroMissionEvent::rewardMission(CUser*, MissionNo::T) —— ORIG T 0x0816748e
//    报告 HeroMissionEvent/rewardMission.md 全文转写。
//    ORIG 入口经 vptr+0x34（CEventBase::IsEventing）判活；本头为布局镜像类，
//    等价读 m_isEventing（头注释口径）。
// ===========================================================================
bool HeroMissionEvent::rewardMission(CUser* user,
                                     HeroMissionCondition::MissionNo::T mission)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：166=166 条、p167 CLEAR(17=17)；
    // 残差=①4.4 槽分配方向（characNo 占 -0x24 而 ORIG -0x18，连带 value/it/end 各浅 4；
    // P212 全头部声明实测 181 条反劣、P50 先声明后赋值无效）⇒ N82/N98 槽序；
    // ②N7 间接 call 目标 %eax(4.4.7) vs %edx(4.1)。
    // [D04 实测] P190 `double framePad; (void)&framePad;` 撑帧：帧 0x44→0x54 但槽位反劣
    // （-0x24/-0x18/-0x14 更远离 ORIG 的 -0x20/-0x1c/-0x18）⇒ 已回滚；维持 N82/N98 判词。
    // [L4b 2026-09-11] 本轮把 characNo 前置到 value 之前：value 归位 -0x24（同 ORIG），
    // characNo 仍 -0x20（ORIG -0x18）、it/end 仍 -0x1c/-0x18（ORIG -0x20/-0x1c）；
    // 「characNo 最前 / value 前 / 分支内声明」三形实测 ndiff 30/30/40 ⇒ 取 30 形。
    // [Q8 2026-09-11 旧N判词重扫·部分推翻] 追加试形④/⑥：characNo 声明再前移到
    //   it(find) 之前 / 函数最开头 —— 两者同码，**ndiff 30→22**（166=166、gap 0、
    //   四数 118/118/1/3/29 逐项不变）⇒ 改善保留（形⑥=函数最开头版）。
    //   残余 3 块 = ①it/end 槽（ORIG -0x20/-0x1c vs 我方 -0x18/-0x14）与
    //   ②characNo 槽（ORIG -0x18 vs 我方 -0x1c）—— characNo 与 it/end 深浅互换：
    //   4.4 恒把 characNo 排在迭代器之前（更深），ORIG(4.1) 相反，声明序四档
    //   （最前/it前/value前/分支内）实测均不能反转 ⇒ 后半维持 N82/N98；
    //   ③varargs 装载的一处寄存器选择（ORIG edx/eax 序 vs 我方 ecx）。
    // 权威口径 NEAR 166/166、ndiff=22（助记符全同）。
    // ORIG 0x0816748e 判活为虚调用 IsEventing(0)
    // （vptr+0x34 = CEventBase::IsEventing），非直读 m_isEventing 字段。
    // [Q8 试形⑥] characNo 移到函数最开头（与「移到 find 之前」的形④同码，ndiff 22）。
    unsigned int characNo;
    if (!IsEventing(0))
    {
        return false;
    }

    // ORIG 顺序（Fix18_E 实证）：user==0 检查在 find 之前（0x32 处），
    // 否则 null 用户会在 find 失败分支执行 get_charac_no/DisConnSig。
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x2d8, "[HeroMission] User is null.");
        return false;
    }

    std::map<HeroMissionCondition::MissionNo::T, BaseHeroMissionCondition*>::iterator it =
        m_conditions.find(mission);
    if (it == m_conditions.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x2df, "[HeroMission] Invalid mission. (charac:%u, mission:%d)",
            (unsigned int)user->get_charac_no(-1), (int)mission);
        user->DisConnSig((DISCONN_SIG)0x3e, true, 0x2e0);
        return false;
    }

    // [L4b 槽序试验] ORIG 槽（深→浅）：value -0x24 / it -0x20 / end -0x1c /
    // characNo -0x18（ORIG 0x08167619 characNo 槽 -0x18，浅于 value/it）。
    // 实测：①头部声明（rewarded/advance 之后）→ characNo -0x24、value -0x20（反）；
    // ②前置到 value 之前 → value 归位 -0x24、characNo -0x20（ndiff 30）；
    // ③分支内声明（P-L11c）→ ndiff 反劣 40 ⇒ 已回滚，保留②形。
    // [Q8 2026-09-11 试形④] characNo 再前移到 it 声明之前（find 调用之前）。
    bool rewarded = false;
    bool advance = false;
    HeroMissionValue* value = 0;
    if (findMissionValue(user, mission, value))
    {
        // ORIG 0x081675f9：missionType 判定与两臂各重新 it->second（ptEv 各一次，
        // P48 无 CSE），不缓存 cond 局部。
        if (it->second->getMissionType() == 3)
        {
            // missionType==3 的条件对象即 TotalMission（ORIG ctor 写 vptr
            // 0x8b7f048 实证）；m_values.find 直接消费（ORIG 无 end 检查）。
            characNo = (unsigned int)user->get_charac_no(-1);
            std::map<unsigned int, std::vector<HeroMissionValue> >::iterator vit =
                m_values.find(characNo);
            BaseHeroMissionCondition* cond = it->second;
            rewarded = static_cast<TotalMission*>(cond)->rewardMission(
                user, *value, vit->second);
        }
        else
        {
            unsigned short before = value->m_fieldA;
            rewarded = it->second->rewardMission(user, *value);   // vtable 槽 +0x0c
            // ORIG 0x081676c5：两个独立守卫（jne/jbe 直跳统一出口，P133）；
            // 第二条件为 > before（无符号 cmp，非 !=0 的 test）。
            if (before == 0)
            {
                if (value->m_fieldA > before)
                {
                    advance = true;
                }
            }
        }
    }

    if (advance)
    {
        processMission(user, (HeroMissionCondition::MissionNo::T)6, 1);
    }
    return rewarded;
}