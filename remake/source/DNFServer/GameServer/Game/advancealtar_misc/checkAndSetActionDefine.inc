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
