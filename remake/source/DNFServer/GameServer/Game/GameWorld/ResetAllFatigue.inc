// [b28d-solo-GameWorld] _ZN9GameWorld15ResetAllFatigueEv verdict=DIFF-N
// b28d-solo 修法：镜像 if —— ORIG 0x086cc3f2 `cmp $0x2,%eax; setle %al; test;
// jne <循环尾>` ⇒ `if (get_state() <= 2) goto L_next;` + 尾标签（P223/AddDailyItem 族），
// 旧 `if (state > 2) {body}` 恒产 setg/je 反向形。
// 残余 N83/N91/N82 封顶：p167 EXTRA _ZnwjPv/_ZdlPvS_（4.1 placement-new 内联 vs
// 4.4 extern call，N83）+ _Unwind_Resume×2/_ZNSsD1Ev×2（EH landing 差异，N91）+
// 帧 0x7c vs 0x8c 槽组平移（N82）。
// [H8B 2026-09-11 复核·终判] 322/305（gap 17）+ p167 66/66（仅 C1/C2·D1/D2 与
// AradServerStateMessage C1/C2 别名）⇒ 无真缺口。全函数仅 2 hunk：①帧：ORIG 0x086cc3ec
// `push %ebp; mov; push %edi; push %esi; push %ebx; sub $0x7c` vs 我方 0x70 且无 push %edi；
// ②浅槽组整平移 0x10（ORIG user -0x20/badge -0x46/trace -0x3c/allocator -0x25/string -0x2c
// vs 我方 -0x10/-0x36/-0x2c/-0x15/-0x1c）：ORIG 0x086cc72a `call getCurSec; mov %eax,%edi`
// （%edi 跨 findGameScript/D1Ev 保活）、bApplied 物化岛 0x086cc750 `test %al; je;
// mov $0x1,%eax; jmp; mov $0x0,%eax; mov %eax,%edi`、4.1 try 内 string 析构标志
// 0x086cc74f `mov $0x0,%esi` / 0x086cc73a `mov $0x1,%esi` + 条件 `_ZNSsD1Ev`——4.4 恒 %ebx
// + EH landing pad。试过：register bool（302→302 零效）、帧 pad 2×double（0x70 对齐；4 个
// 过头 0x80）、badge 聚合初始化 `= {0}`（已落地 0x086cc40a 4+2 字节形）⇒ 判
// N7（%edi/%ebx）/N82（槽组）/N126（首槽）/N104（EH 尾块相对序）封顶。
void GameWorld::ResetAllFatigue()
{
    // [L11·round1 判词更新] 帧 0x70 vs ORIG 0x7c 实测复核：4.4 带 call 帧恒 16k+4 量化
    // （P190 附注②，本函数族 0x44/0x54/0x64/0x74… 可达），ORIG 的 0x7c **非量化值**
    // 任何 pad 组合不可凑（2 个取址 double 已达 0x70，4 个过头到 0x80）⇒ N 封顶。
    // 其余 0x10 整帧槽平移（-0x20/-0x1c/-0x3c vs -0x10/-0xc/-0x2c）为避免 N126/N82，
    // 且缺 ORIG 的 `push %edi`（ORIG 用 %edi 承载 getCurSec/bApplied，4.4 恒 %ebx；
    // 裸 register 形实测 302→302 零效）⇒ 维持 N。
    static bool isFirst = true;
    // [b28d-sweep] P190 撑帧实测（推翻旧「double framePad 零效」判词）：ORIG 帧
    // 0x7c/深槽组（badge -0x46 / iterator -0x4c / msg -0x58）比我们深 0x10；
    // 2 个取址 double 撑帧 0x60→0x70，深槽组三处逐一对齐（ndiff 125→110）；
    // 4 个则过头（帧 0x80、badge -0x56，ndiff 回 125）。浅槽组（user -0x20/active
    // -0x1c/trace -0x3c/string -0x2c/allocator -0x25）仍浅 0x10 = N126 首槽常数 8
    // + 4.1 匿名 return-temp 专属槽，属 N82/N126 封顶。
    double framePad0;
    (void)&framePad0;
    double framePad1;
    (void)&framePad1;
    // [b28d-solo-GameWorld] _ZN9GameWorld15ResetAllFatigueEv verdict=DIFF 322/307
    // N7+N82+N104 封顶。本轮修法（均以 ORIG 0x086cc3ec 段实证）：
    //  ① 字面量直传 findGameScript(std::string)（0x086cc71e 单次 C1EPKc）——删具名
    //     name 局部后 p167 EXTRA _ZNSsC1ERKSs+_ZNSsD1Ev×2 全消（73→66 calls CLEAR）；
    //  ② 栈对象 AradServerStateMessage msg（0x086cc7c8 直 call C1，无 _ZnwjPv）——
    //     placement-new EXTRA 归零；
    //  ③ charging 内联实参（0x086cc46e movswl 直传，P0/P200）；
    //  ④ active 改 int 具名局部（0x086cc494 cwtl+movl 槽往返）；
    //  ⑤ cMyTrace 全表达式临时链（P227）复现 ctor 前求值 varargs；
    //  ⑥ bApplied 条件置位形复现 ORIG 物化岛。
    // 残余（不可造）：%edi↔%ebx 分配差（ORIG getCurSec/bApplied 用 %edi ⇒ push/pop
    // %edi 2 条；4.4 恒 %ebx；register bool 形实测 302→302 零效）N7；浅槽组 0x10 浅移
    // + 帧 0x7c vs 0x70（push edi 4 + 8）N82/N126；isApplied 后 EH 尾块相对序 rotate
    // N104；m_actionPoint receiver 多 2 条 add 寻址（N107 族）。
    // [b28d-solo-GameWorld] ORIG 0x086cc40a `movl $0,-0x46` + `movw $0,-0x42`（4+2 字节）
    // ⇒ 试聚合初始化 `= {0}`（P202①/P214③ 展开形）。
    short badge[3] = {0};
    SetWeekendBounsEvent();
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // b28d-solo：ORIG 0x086cc3f2 段 `cmp $0x2,%eax; setle %al; test %al,%al;
        // jne <循环尾>` ⇒ 镜像 if（<=2 早退）+ 负条件 goto 尾标签（P223/AddDailyItem 族）。
        if (user->get_state() <= 2)
        {
            goto L_next;
        }
        {
            // [b28d-solo-GameWorld] ORIG 0x086cc46e 段：ChargingFatigueBattery 结果
            // `movswl %ax,%edx` 直传（无槽，P0/P200 内联实参）；具名 short 局部会落槽。
            user->IncFatigueBatteryCharging(
                GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, 0));
            // ORIG 0x086cc494 `cwtl; mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp)`
            // ⇒ active 是 int 具名局部（非 short）。
            int active = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
            if (active != 0)
            {
                user->DecFatigueBatteryCharging(active);
                // [b28d-solo-GameWorld] P227 全表达式临时链：ORIG 0x086cc4be/0x086cc4cc
                // 在 ctor **之前**右起求值 varargs（%esi=GetFatigueBatteryCharging、
                // %ebx=getCurCharacNo）并跨 ctor 保活；具名 trace 对象在 4.4 恒 ctor 后求值。
                cMyTrace(__PRETTY_FUNCTION__, 0x1286, 0)(
                    "FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
                    user->getCurCharacNo(), active, user->GetFatigueBatteryCharging());
            }
            user->DBUpdateDBLogItem();
            user->ResetDBLogItem();
            user->RecoverFatigue(0);
            user->SendFatigue();
            user->resetCurCharacMemberBonusFatigue();
            user->resetAccountMemberBonusFatigue();
            user->resetAccountUsedFatigue();
            user->SetDailyBadge(badge);
            user->ResetDailyCharacExpandData();
            ((CBoosterGage*)user->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0x11))->send_data(user, 0);
            user->send_MissionList();
            user->ClearCurConditionEventStep();
            user->ClearCurConditionEventRewardStep();
            user->SendConditionEventInfo();
            user->ClearProperDungeonClearCount();
            user->SendProperDungeonClearCount();
            user->ResetRevengeDungeonClear();
            user->resetNPCRelationShipDailyData();
            user->sendNPCRelationShipFavor();
            user->getDungeonGainedGold()->reset();
            APSystem::DB_UpdateActionPoint::makeRequest(user->GetUID(),
                *(const APSystem::_SIG_LOAD_ACTION_POINT*)user->m_actionPoint.GetActionPoint(), true);
            APSystem::CUserProc::SetTodayActionAndCheckMedalReward(user);
            user->GetGoldControl()->SavetoDB(false, true, false);
            ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
                ->send_event_init_data(user, true);
            user->resetBlueMarbleEnterCount();
            user->sendBlueMarbleEnterCount();
            user->ResetCurCharacUsedGiftFatigueQuantity();
            // ORIG 0x086cc750-0x086cc760 `test %al; je; mov $1,%eax; jmp; mov $0,%eax;
            // mov %eax,%edi` ⇒ bApplied 为 callee-saved 寄存器 bool（跨 string D1 保活，
            // 无槽）。P248 裸 register 三条件全满足（P231/P307）。
            // [H8B 2026-09-11 实验·P248 档③·负结论] register bool → register int 实测：
            // 322/305 与行级 diff 465 行逐值不变（4.4 对 bool/int register 同折叠
            // `mov %eax,%ebx`，ORIG 0x086cc750 的 `test %al,%al; je; mov $0x1,%eax; jmp;
            // mov $0x0,%eax; mov %eax,%edi` 物化岛不可造）⇒ 类型档无效，已回滚 bool 形。
            register bool bApplied = false;
            if (isFirst)
            {
                // [b28d-solo-GameWorld] P52/P134 链式单语句：ORIG 0x086cc6f2 getCurSec
                // 先求值存 %edi（跨 Get/findGameScript 保活），字面量直构按值 string
                // 临时（0x086cc71e 单次 C1EPKc，无 _ZNSsC1ERKSs）；具名 name 局部会多发
                // 一次拷贝构造 + 2 次析构（p167 EXTRA 实证）。
                if (static_cast<ARAD::Arad_EventPeriodDataManager*>(
                        ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                            std::string("Arad_EventPeriodDataManager")))
                        ->isApplied((ENUM_REPEAT_EVENT_CODE)0x8e,
                                    GlobalData::s_systemTime_.getCurSec()) != 0)
                {
                    bApplied = true;
                }
                else
                {
                    bApplied = false;
                }
            }
            if (bApplied)
            {
                // ORIG 0x086cc7c8 `lea -0x58(%ebp),%eax; call AradServerStateMessage::C1`
                // = 直接栈对象（无 placement-new extern call/landing，N83）。
                AradServerStateMessage msg;
                ARAD::notifyOpenMessageDialog(user, 0, 0x8e, msg);
            }
            user->m_field8ec32 = 4;
            InterSelectPcroomDailyReward::SendPacket(user);
            ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
                user, (HeroMissionCondition::MissionNo::T)5, 0);
        }
    L_next:;
    }
    if (!m_UserInWorld.empty())
    {
        isFirst = false;
    }
}