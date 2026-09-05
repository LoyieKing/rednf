// STRecovery：恢复数据（ST 序列化/脚本结构，"[hp recovery]"/"[mp recovery]" 解析）。
// ORIG 导出符号：C1 W 0x089c3098（仅调用 clear）、clear W 0x089c30ac。
// 布局按 ORIG clear 反汇编（0x089c30ac）：
//   +0x00 movb $0（1 字节）、+0x04/+0x08/+0x0c movl $0（int）、+0x10 movb $0（1 字节）。
//   sizeof=0x14：STStackableScript 中两个 STRecovery 成员（+0x2b0/+0x2c4）相隔 0x14 实证；
//   ImportStackableScript（+0x2b0 区 movb/movl/ScanInt 写入，+0x2c4 区同形）佐证字段类型。
#ifndef DNF_GAME_STRECOVERY_H_
#define DNF_GAME_STRECOVERY_H_

class STRecovery
{
public:
    STRecovery();   // ORIG W 0x089c3098（C1/C2 同址）：仅调用 clear()
    void clear();   // ORIG W 0x089c30ac

    // ORIG 无访问控制；ImportStackableScript 依报告直接写五个字段
    // （"[hp recovery]"/"[mp recovery]"：m_bAddMode="+" 0/1、m_nValue1/2=ScanInt、
    //  m_nTarget="myself" 0/1、m_bEnable=1），故按 ORIG 用法公开。
public:
    unsigned char m_bAddMode;  // +0x00（ORIG movb $0；Import："+" 判断 → 0/1）
    int m_nValue1;             // +0x04（ORIG movl $0；Import：ScanInt）
    int m_nValue2;             // +0x08（ORIG movl $0；Import：ScanInt）
    int m_nTarget;             // +0x0c（ORIG movl $0；Import："myself" 判断 → 0/1）
    unsigned char m_bEnable;   // +0x10（ORIG movb $0；Import：movb $1 置位）
};

#endif  // DNF_GAME_STRECOVERY_H_
