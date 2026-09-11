// [b28d-solo-CEventScriptMng] process_account_first_login：NEAR — N7 封顶。
// ORIG 0x0810bde0：两条独立守卫（P133）+ 匿名 bitset reference 临时（槽 -0x14）；
// 修复前合并 `||` 触发 bool 汇聚（mov$1/jmp/mov$0/test）⇒ 拆两条 if 后守卫岛
// （nop;jmp + nop）与全帧槽位逐条对齐。
// 残差唯一（[L6S3b 2026-09-11 复核更正]）：尾部虚调用段寄存器轮转 —— ORIG
// `mov (%eax),%edx`（目标入 %edx）+ 实参/this 均用 %eax；我方（**本 TU 已走
// 446 后端** = dnf_game_cxx.sh:650 `exec "$C6" -B cc1plus446bin/`）为
// `mov (%eax),%ecx`（目标入 %ecx）+ 实参用 %edx、this 先入 %eax ⇒ 即使 446
// 后端也未复现 ORIG 的寄存器角色（N7 族，源码无解；446 路由本身是该族最优
// 后端，保留不改）。p167 CLEAR（239 口径逐函数：6=6，仅 D1/D2 同址别名），
// 语义闭合。
// [L6S5 复核] 读数不变（65/65 NEAR；p167 6=6，仅 C2/D2 同址别名）。本波新钥匙
//   逐条评估（未落地者附理由）：P231 裸 register ✗（ORIG 该实体**有**具名槽
//   -0xc，加 register 会去槽）；P248 类型三档 ✗（已按 ORIG：bitset<32> 按值 =
//   单 dword 0x14(%ebp)、user 0x10(%ebp)）；P226 语句表达式 ✗（必然引入 ORIG
//   不存在的临时槽 ⇒ 结构性劣化）；P326 ✗（本函数无迭代器对）。⇒ 维持 N7。
void CEventScriptMng::process_account_first_login(
    int key, CUser* user, std::bitset<32u> flags)
{
    // P133 独立守卫；confirmFlag 无栈槽（P0：值经 %edx 直传 operator[] 实参）。
    if (user == 0)
        return;
    if (!is_eventing(key))
        return;

    if (!flags[get_convert_to_user_confirmflag(key)])
    {
        CEventEntity* entity = get_event_entity(key);
        if (entity != 0)
            static_cast<CAccountFirstLogin*>(entity)->do_action(
                key, user, flags);
    }
}