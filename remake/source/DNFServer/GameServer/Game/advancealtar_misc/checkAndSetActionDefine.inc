// ORIG T 0x889a032（报告反编译全文）：回填 m_define；找不到 → printf + 0；
// 全部成功则写 summary（(phaseIdx, 累计时长起点, phase type)）并把累计时长存
// TimeLineSummary::m_field0（TimeLine+0xc），返回 1。
// [b28d-solo-advancealtar_misc 判词] DIFF→N 封顶（语义/调用点/控制流全等）：
//   已修①**static 成员**（ORIG 首参 timeLine 读 0x8(%ebp)、无 this；原非 static 声明
//   使 timeLine/defines 落在 0xc/0x10、整帧 +4 错位，见 CDataManager.h 声明处）；
//   ②_PhaseSummary::m_pad 字段化 + 逐字段直写（原 fields[3]+memcpy 多一次 memcpy 调用）；
//   ③`vector<_ActionAssign>& assigns = pit->m_assigns;` 复现 ORIG -0xc 地址缓存；
//   ④两循环后置 `++`（ORIG ppEi 0x0889bede/0x0889bf48 带 sret 临时）与**体内
//   `pit++` + 体末 `++phaseIndex`**（ORIG 0x0889a1d0 位于 add 之后、addl 之前）。
//   残余（N82附/跨版本）：dit(-0x34) 与 summary(-0x40) 槽互换——4.4 聚合对象优先占
//   低槽（summary 最深、dit 次深），4.1 按生命周期穿插（dit -0x40、summary -0x3c）；
//   其余槽（pit -0x2c/end -0x28/m_assigns.end -0x24/defines.end -0x20/ait ppEi -0x1c/
//   pit ppEi -0x18/totalDuration -0x14/phaseIndex -0x10/assigns -0xc/ait -0x30）全等；
//   全头部声明法对非平凡 summary 不可用。
//   [C07ExpertHeroAssault 2026-09-10 · P317 落地] 旧判词「EXTRA =
//   `_PhaseSummary::~_PhaseSummary()`×2 + `_Unwind_Resume`×1，ORIG 该 dtor 符号
//   0x0889ebc0 存在，按 P12 不可删」**已作废**：按 P317 删 `advancealtar.h` 的
//   用户空析构后，`nm /tmp/tu_advancealtar_misc.o` 证 `~_PhaseSummary()` 仍为
//   W ×2（使用点发射）⇒ 未丢符号。读数改前→改后：our_insns 155→**138**
//   （ORIG 138，gap −17→**0**），verdict DIFF→**NEAR**（残余 = dit(-0x34) 与
//   summary(-0x40) 槽互换 + 4.4 聚合对象优先占低槽 = N82附；无 push esi、无
//   `_Unwind_Resume`、无 dtor call 残差）。p167：MISSING 0 / EXTRA 0。
int StageTimeLineParameter::checkAndSetActionDefine(
    TimeLine& timeLine, const std::map<int, ActionDefine>& defines)
{
    // b28d-solo-advancealtar_misc：ORIG @0x0889a032 全函数**不复用** Phase&
    //   （每处 `pit->m_*` 重新 ptEv：0x0889a061/a185/a1ab），原具名 `Phase& phase`
    //   使 4.4 缓存 phase 指针于 -0xc；改逐点 `pit->m_*` 后 -0xc 复现 ORIG 的
    //   `&pit->m_assigns` 缓存形（`call ptEv; add $8,%eax; mov %eax,-0xc`）。
    //   summary 由 fields[3]+memcpy 改具名字段直写（_PhaseSummary::m_pad 已字段化）。
    //   ORIG 两循环增量段均为 **后置** `operator++(int)`（Phase ppEi 0x0889bede /
    //   _ActionAssign ppEi 0x0889bf48，各带独立 sret 临时）⇒ 源写 `pit++`/`ait++`；
    //   ORIG -0xc 缓存 `&pit->m_assigns`（`call ptEv; add $8,%eax; mov %eax,-0xc`）
    //   ⇒ 原码有 `std::vector<_ActionAssign>& assigns = pit->m_assigns;`（P0 槽往返）。
    // ORIG 外层为「体内 `pit++` + 体末 `++phaseIndex`」（0x0889a1d0 ppEi 位于
    //   `add %eax,-0x14` 之后、`addl $1,-0x10` 之前）⇒ 原码非 for 增量位。
    int totalDuration = 0;                                         // local_18
    // [L13-group6 判据⑩「先声明+延后赋值/后声明带初值」] ORIG 0x0889a0xx：
    //   totalDuration 初始化在函数头（`movl $0x0,-0x14`），phaseIndex 的
    //   `movl $0x0,-0x10` 落在 `begin()` call **之后** ⇒ 原码 pit 初始化语句
    //   位于两者之间；我方把两 meminit 都提前（4.4 meminit 顺序 = 声明序）。
    std::vector<Phase>::iterator pit = timeLine.m_phases.begin();
    int phaseIndex = 0;                                            // local_14
    for (; pit != timeLine.m_phases.end(); )
    {
        std::vector<_ActionAssign>& assigns = pit->m_assigns;
        for (std::vector<_ActionAssign>::iterator ait = assigns.begin();
             ait != assigns.end(); ait++)
        {
            std::map<int, ActionDefine>::const_iterator dit = defines.find(ait->m_field0);
            if (dit == defines.end())
            {
                std::printf("advancealtar : not found ActionDefine Id = %d\n",
                            ait->m_field0);                        // [objdump printf 格式串]
                return 0;
            }
            ait->m_define = const_cast<ActionDefine*>(&dit->second);   // +4 回填
        }

        {
            _PhaseSummary summary;                                 // [objdump ctor -0x3c]
            summary.m_startDuration = totalDuration;               // +0x04（ORIG 先写）
            summary.m_phaseIndex = phaseIndex;                     // +0x00
            summary.m_phaseType = pit->m_field4;                   // +0x08（ptEv 形）
            timeLine.m_summary.m_phaseSummarys.push_back(summary); // push param_1+0x10
        }

        totalDuration += (int)pit->m_field0;                       // [objdump 本轮时长累加]
        pit++;
        ++phaseIndex;                                              // local_14
    }
    timeLine.m_summary.m_field0 = totalDuration;                   // [objdump *(int*)(param_1+0xc)]
    return 1;
}