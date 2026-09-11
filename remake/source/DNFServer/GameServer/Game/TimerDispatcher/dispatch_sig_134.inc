// [b28d-30] 复核 DIFF：极性/短路已修（真 bug 修正）；残余 `jne then; jmp end` 块排布差
// 为 N93附/P223（4.4 恒产 je 跳过镜像），ORIG 0x0863aa90。
// [b28d-solo-TimerDispatcher] 判词 N93附/P223 封顶：`jne then; jmp skip` 块序（4.4 恒
// `je skip` 镜像）；P272 空 then+else 试形反产 sete（已回退）（52/51）。
bool Timer_Send_Ontime_Reward::dispatch_sig(int /*p1*/, int /*p2*/, memberIdentificationNumber_t /*p3*/)
{
    COnTimeEvent* ev =
        (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // [b28d-30] ORIG 0863aaae..0863ab03：正极性短路 `ev != 0 && IsEventing(0)`
    // （双 je 同落 +0x49 的 mov $0 物化岛）⇒ if/else 两臂形；旧码 `ev==0 ||
    // IsEventing==false` 负极性 + bool ok 槽物化（多 xor/槽段，指令 59 vs 52）。
    if (ev != 0 && ev->IsEventing(0))
    {
        // ORIG +0x58: process_send_reward_to_user()
        ev->process_send_reward_to_user();
        // ORIG +0x63: get_ontime_reward_user_count()
        // [TimerDispatcher-3 判据①] ORIG 0863ab15 `jne [尾 body]` + 0863ab17
        // `jmp [尾 return 1]`：真分支跳**函数尾部** body（else 块之后）、假分支 jmp 尾。
        // 双 goto 复现形实测：`if(==0)goto L_done; goto L_regist;` 结构对齐（je 目标
        // 同为 body）但 4.4 反极性成 `sete;je` + L_regist 前 nop，差异 2 条 > 裸 if 的
        // 1 条（je↔jne，多重集仅此一处）⇒ 回滚保留裸 if（N93附/P223 镜像封顶）。
        // [A12-2026-09-10 翻案实测·失败] 正极性 `!=0` 双 goto + 标签后置
        // （`if(!=0) goto L_regist; goto L_join;` + `L_regist: RegistNextTimer();`
        // `L_join: return 1;`）实测 4.4 产物 54 条：条件跳仍反转成 `je <join>`、
        // 且 call 后多插 `jmp <join>; nop`（4.4 把标签块划为独立块）⇒ 比裸 if 的
        // 52 条多 2 条，已回滚。4.4 的 return-合并/块布局与 4.1 不同（ORIG 假路径
        // `jmp <+0xb5 join>` 为 4.1 共享出口 mov 合并产物，4.4 恒发 per-return mov）
        // ⇒ 维持 N93附/P223 封顶。
        if (ev->get_ontime_reward_user_count() != 0)
        {
            RegistNextTimer();
        }
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1638, 5);
        trace("[ON_TIME_EVENT] Timer_Send_Ontime_Reward() error");
        return 0;
    }
    return 1;
}