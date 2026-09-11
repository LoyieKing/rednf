// ORIG _ZN15cUserHistoryLog6ExpAddEii13eExpAddReasoni @0x08684bd0：
//   仅当 reason>0xc 或 (1<<reason)&0x1892==0 时记录（排除常规来源）；
//   日志第三参数与判断均用 reason（reasonIdx 未参与）。
// [b28d-solo-cUserHistoryLog] DIFF（orig 32 / our 33）：尾岛已按 P223 负条件 goto 尾空标签复现
// （ORIG 0x08684c1f `jmp +0x52; nop` 空 else 占位岛）；残差仅 N18a 反向
// （ORIG 复用 cmp 的 %eax 作 shl 源，4.4 对参数表达式二次使用恒重载
// `mov 0x14(%ebp),%eax`；e7/e9/e10/语句表达式/一元加四形探针实测均重载）。
// [2026-09-09 复核] 补测 register 形参/register 局部/语句表达式/逗号表达式/有符号移位
// 等 6 形（/tmp/hlprobe/exp2/exp3/exp4）：register 形参可消重载但寄存器角色全变
// （%eax=reason / %ebx=常量 / %esi=结果 + push esi），其余仍重载 ⇒ N18a 反向封顶。
// [SweepHistoryLog2 2026-09-09 三度复核] 再试 10 形（/tmp/exp_probe.cpp f1..f14，c6root 4.4.7
// 与 -B cc1plus446bin 双编同结论）：①(unsigned)reason 两侧同写；②三目 A?true:C；
// ③反向 !(A&&B)；④正向 A>=..&&..；⑤1u→1（折 sar/and $1）；⑥去 cast（折 jg，证 enum 底层
// signed，须保留 (unsigned)）；⑦register unsigned 局部（落 %ebx +2 条）；⑧register enum 局部
// （+2 条）；⑨语句表达式 ({reason;})；⑩逗号 (0,reason)——**全部仍多 1 条
// `mov 0x14(%ebp),%eax`**。机理：4.1 把 || 左操作数装载的 %eax 保留给右操作数 shl 源；
// 4.4 -O0 无 CSE，第二次使用恒重读形参槽 ⇒ 无合法源形可造 ⇒ N18a 反向封顶（ORIG 31/我们 32）。
void cUserHistoryLog::ExpAdd(int exp, int sp, eExpAddReason reason, int reasonIdx)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N6/N18a）→ 试=②prett ③strall CLEAR；
    //   ④旧 10 形探针（register 形参/局部、语句表达式、逗号、(unsigned) 两侧同写、去 cast、
    //   三目反向等）+ 本波 n1_v 复核：仍多 1 条 `mov 0x14(%ebp),%eax`（4.4 -O0 无 CSE，
    //   reason 参数二次使用恒重载；ORIG 4.1 复用 `cmp $0xc` 装载的 %eax 作 shl 源）
    //   ⇒ 维持 N18a（反向封顶，32/34 gap −2）。
    // [b28d-solo-cUserHistoryLog] N6 翻案：`(int)` cast 阻断 4.4 前端的 bool 位测试折叠
    // （探针 /tmp/hlprobe/Game/probe4.cpp k1：`(int)((1u<<r)&0x1892u)==0` 产 ORIG 的
    // shl;and $0x1892;test，无 cast 恒折 shr;and $1）。
    // 证据（ORIG 0x08684bd7..0x08684c27）：`mov 0x14(%ebp),%eax; cmp $0xc,%eax; ja body;
    // shl/and/test; jne <nop 岛>; body: ... call; jmp +0x52; nop`。P223 尾空标签复现
    // （探针 probe4.cpp e3 与 ORIG 同形，仅 reason 重载一条差）。
    if (!((0xc < (unsigned)reason) ||
          (((int)((1u << reason) & 0x1892U)) == 0)))
        goto end;
    m_user->LogHistory("Exp+,%d,%d,%d", exp, sp, reason);
end:;
}