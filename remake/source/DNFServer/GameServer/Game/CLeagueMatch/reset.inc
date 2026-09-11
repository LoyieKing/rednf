// [b28c-21] P210 定义顺序：ORIG TeamInfo::reset `sub $0x28` 保守帧；4.4 对
// 已见定义的 PlayerInfo::reset 精确继承 $0x14 ⇒ 本定义前置于 callee 之前。
// [A11-round5 复核] N125 证据加强（本 TU 12 件的同一残差族）：本轮以 8 源形探针
// （`&arr[i]` 索引 / `(char*)t + i*0x1c` 指针算术 / 具名 int 中间量 / 二维下标 /
// unsigned 索引 / 数组退化 + 标量偏移）在 4.4.6 逐形复编（/tmp/r5/probeD.cpp f1..f8），
// 恒产 `shl $0x2,%eax; lea 0x0(,%eax,8),%edx; sub %eax,%edx`，**无一**产 ORIG 的
// `lea 0x0(,%eax,4),%edx; lea 0x0(,%edx,8),%eax; sub %edx,%eax`（TeamInfo::size
// 0x0855cd42、GetUser 0x0855e5f4、GetGap 0x0855e1f9、CheckMatchable 0x0855fa2a、
// OnLeaveRoom 0x0855d2xx、_RegisterWaitingRoom 0x0855d94b 等族）⇒ N125 结构性豁免。
// 追加 4 形（probeD2.cpp f9..f13：成员数组元素取址直传 out-of-line callee / `arr+i`
// 指针加法 / `arr[i].field` 字段访问 / `reinterpret_cast<Ent*>((char*)t+i*0x1c)` 偏移链）
// 仍恒 `shl $0x2` ⇒ 12 源形全败，后继勿再试。
// [A11-round5 C05/D08 复查 · 无落点] ①范式①（尾 fail 块）：本 TU 18 件 DIFF 的 ORIG
// 尾 16 条独立 `mov $0/$1,%eax` 均为正常返回值块（GetUser/GetSchoolNo/OnMatchRoom/
// CheckMatchable/TeamInfo::add·del/_getSameSchool 等），我方同形；②范式③（早返中继岛）
// 与 D08（多守卫同目标合并）：全 33 件 diff 的 `jmp` 行差逐件为 0 ⇒ 无岛可消；
// ③范式②（F2）：「ORIG `cmp mem,reg`」行（TeamInfo 族 stride 链）两侧同为内存直比，
// 残差仅槽号/lea-vs-shl，非装载序 ⇒ 不适用。
// [b28d-solo-CLeagueMatch] DIFF 19/20·N-STRIDE（S180 定稿）：stride 0x1c 的 i*4
// ORIG 用 lea(,%eax,4)、4.4 恒 shl $2（探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）⇒ 4.4 多 1 条 mov %edx,%eax
// 中转；语义等价，结构性豁免。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局 · 全族统一证据/勿再烧轮次]
// 本 TU 非 identical 16 件的残差已全量归因，两条结构性来源：
//  (a) N-STRIDE（12 件）：ORIG `lea 0x0(,%eax,4),%edx`（×4 用 scale=4 的 lea）vs
//      4.4 恒 `shl $0x2,%eax`（+1 条中转 mov）。
//  (b) synth_mult copy 链（3 件：OnCompleteLoad/_SendMatchStatePacket/OnReward）：
//      ORIG `mov %ecx,%eax; mov %eax,%ecx`（纯冗余，删去逐条等价）。
//  本轮新增实证（全部失败）：`#pragma GCC target` 8 形（tune=i386/i486/pentium/
//  pentiumpro · arch=i386/i486/pentiumpro · tune=generic）、`#pragma GCC optimize`
//  3 形（strength-reduce / O1 / web）、cc1plus **4.4.6 后端** A/B（头集 4.4.6 +
//  -march=i486，独立 /tmp/tu_CLeagueMatch_446.o，逐件读数与本 TU 全同）、以及
//  70 源形探针（/tmp/probe_ocl{1,2,3}：数组/指针/char* 偏移链/sizeof/两层引用/
//  具名中间量/register/unsigned·cast 索引/上界写法/加法序）⇒ 4.4.x 全系（含 O1）
//  不产该两类形；ORIG 侧应为 4.1 系后端产物。
//  **已试尽**：源码形/函数级 pragma/后端版本三维。**未试**：换 cc1plus 到 4.1/3.4
//  变体（本 TU 当前路由 `$C6`+i486，属 dnf_game_cxx.sh 路由/Main 决策，禁改）。
//  语义轴：16 件 p167 全绿（12 CLEAR + 4 件仅 C1/C2·D1/D2 同址别名），另修复
//  OnCreateRoom 韩文日志串实字节（UTF-8→cp949，§6.5，ORIG 文件偏移 0xb7a2c8/0xc5a23c）。
void TeamInfo::reset()      // ORIG T 0x0855cb16
{
    for (int i = 0; i <= 3; ++i)
        m_players[i].reset();
}