// ===========================================================================
// CAutoMarketConditionsControl::LoadDatabase  ORIG T 0x082f8684
// （LoadDatabase.md 全量转写）：
//   基准值 = msg+0x68 非 0 ? (msg+0x0,+0x4) :
//             ((msg+0x44,0x48)+(msg+0x3c,0x40)) - ((msg+0x18,0x1c)+(msg+0x10,0x14))
//             + (msg+0x20,0x24)；
//   三次 GetMarketConditionRate（gold/equip/dur 段）；msg+0x38 变化则通知；
//   仅 channel no==1 时 DB_UpdateAutoMarketConditionsControl 上报。
// P0 判据修正（batch-12）：ORIG 帧（sub $0x9c）无 amount/field30/34/38/v1/v2
// 槽——base 为唯一 64 位局部（-0x28/-0x24，有槽：三处 fildll -0x28 复读 +
// trace/diff 复读 ⇒ 具名局部），(double)msg+0x8 在 3 个调用点各自内联转换
// （每点 `mov msg; mov 0x8/0xc; mov scratch pair; fildll; test 高位; jns;
// fldt 2^64; faddp` = 4.1 utodf 形），phase 值直接进参数寄存器（%ecx 单槽
// 即弃）；msg+0x38 在调用参数与 dur 变化比较两处分别重读（O0 无 CSE，P48）。
// base/diff 为 signed long long：ORIG base 转 double 是裸 fildll（无符号
// 修正），diff = 整数 sub/sbb，(ll)diff<0 经 `cmpl $0,-0x1c; jns` 高字判负。
// trace 变参：ORIG 4.1 在 cMyTrace ctor call 前把 %qd/%u/%d 全部求值入
// 槽（-0x64=+0x3e8、-0x60=+0x380）/callee-saved（edi=this[0]、ebx/esi=
// msg+0x8/0xc）（N80 跨版本装载时刻；本侧两具名局部+其余表达式，push 段
// 指令等量，寄存器归属差记 N）。
// [b28e 定稿] 248/246（改前 250/246）；残差 2 条 =
// ①trace msg 双字段 4.1 共享单次基址装载、4.4 每局部重读（+1，N18a 族）；
// ②push 段 4.4 寄存器中转多一跳（+1，N7）。
// **本轮已修 −2**：`long long diff = base - (long long)msg->m_view.m_optimumGold`
// 的**冗余 cast** 是 4.4 RHS-first 预装载（`mov 0xc(%ebp),%eax; mov 0xc(%eax),%edx;
// mov 0x8(%eax),%eax; mov %eax,%ecx; mov %edx,%ebx; mov -slot,%eax; mov -slot+4,%edx;
// sub %ecx,%eax; sbb %ebx,%edx`，多 2 条 regmove）的根因（P41 变体：32 位平台对
// 同宽 signed↔unsigned 的显式 cast 触发 4.4 转换中转）；删 cast 写
// `base - msg->m_view.m_optimumGold` 后为 `mov -slot,%eax; mov -slot+4,%edx;
// mov 0xc(%ebp),%ecx; mov 0xc(%ecx),%ebx; mov 0x8(%ecx),%ecx; sub %ecx,%eax;
// sbb %ebx,%edx` = ORIG 4.1 的 LHS-first 形，逐条对位（250→248）。
// 另：`register unsigned long long optGoldNow = …; (int)(optGoldNow>>32)` 拆字形
// 实测 **252**（多 shift/槽化）⇒ 已回退记 N；帧差 0x10（0xac vs 0x9c）与槽号整体
// 偏移属 N82/N126（版本首槽基址常数）。语义（base 唯一 64 位局部/三处内联 utodf/
// 整数 diff 减法/clamp/channel==1 上报参数）与 ORIG 逐条一致。
// [P1T3LinkMiscSecuTimerSolo 2026-09-11 本轮实测定稿] 248/246 → **247/246**
// （gap −2 → −1，blocks 33 不变）：删 trace 第 3 实参的
// `static_cast<int>(optGoldHi)` 冗余 cast（`%d` 实参位模式同 ⇒ 语义零变）。
// ORIG 82f88a3/82f88a7 为 `mov %ebx,0x10(%esp); mov %esi,0x14(%esp)` 直装
// callee-saved；原 cast 形在 4.4 多一条 `mov %edi,%eax; mov %eax,0x14(%esp)`
// 中转（RHS-first 转换）⇒ 去 cast 后逐条对位。
// 剩余 +1 = trace 段 msg 双字段基址重装：ORIG 82f8865-82f886b 单次
// `mov 0xc(%ebp),%eax; mov 0x8(%eax),%ebx; mov 0xc(%eax),%esi`，我方 4.4 对两个
// 32 位视图读各发一次 `mov 0xc(%ebp),%eax`。`register unsigned long long` 拆字、
// 64 位具名局部均再劣（落 8B 槽 + 存/读对）⇒ N18a/N7 封顶；其余 32 块为帧 +0x10
// 与槽号整体偏移（4.1 0x9c vs 4.4 0xac）⇒ N82/N126，非源码可调。
// ===========================================================================
// ===========================================================================
// Inter_MonitorServerEventStart::SendEventNotiPacket  ORIG T 0x084cf2de
// 构造 header(0,0x45) + byte(noti) + short(eventIdx) + short(interval) 包；
// user 为空广播全服（GameWorld::send_all），否则单发（CUser::Send）。
// ===========================================================================
void Inter_MonitorServerEventStart::SendEventNotiPacket(bool noti,
                                                        short eventIdx,
                                                        short interval,
                                                        CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x45);
    packet.put_byte(noti);
    packet.put_short((int)eventIdx);
    packet.put_short((int)interval);
    packet.finalize(true);
    if (user == 0)
        G_GameWorld()->send_all(packet);
    else
        user->Send(packet);
}