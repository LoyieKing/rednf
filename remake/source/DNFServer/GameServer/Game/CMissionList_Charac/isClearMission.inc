bool CMissionList_Charac::isClearMission(int index) const
{
    // b28c-L5：ORIG 0x085e50fa 尾形 `mov $0x1,%eax / jmp / mov $0,%eax / leave`（20 条）。
    // 4.4 对「if(cond) return 1; return 0;」恒额外物化一段 bool 归一岛（+5 条：
    // `mov $0,%eax; test %al; je; mov $1; jmp; mov $0`），探针 /tmp/l5_probe_isclear.cpp
    // fA(异形 25)/fB(三目 20 全等)/fD(嵌套 if 20 但条件树异形) 定案：三目返回形
    // 与 ORIG 逐条全等（P47 三目 vs if-return 块排布镜像的本例应用）。
    return ((index >= 0) && (index < 0x100) && m_clearedFlags[(unsigned int)index])
               ? 1
               : 0;
}