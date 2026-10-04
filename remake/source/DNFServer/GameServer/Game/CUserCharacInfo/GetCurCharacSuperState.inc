// [S1Charac 2026-09-11 类型轴复核 · 非本写域可修] ORIG 0x0822f6f4 段共 13 条：
//   0x0822f707 movzbl 0xe65(%eax),%eax → 0x0822f70e jmp → 直接返回字段原值，
//   **无 test/setne 归一**；我方 16 条多出 `test %al,%al; je; mov $0x1,%eax`
//   （bool 返回在 -O0 必做 0/1 归一）。4.4.6 cc1plus 探针（/tmp/s1_ss_probe.cpp，
//   -O0 -m32）逐条复现：`bool A` → 16 条含 testb/je/movl $1；`unsigned char B`
//   → 13 条与 ORIG 同形。⇒ 根因 = 返回类型 bool（CUserCharacInfo.h:964，K1Dispatchers
//   本波 uchar→bool），而字段 m_superState 本身即 unsigned char（h:484）。
//   函数体层面无解（签名轴，改必须动 .h）⇒ N（类型轴），已上报父泳道。
// [P8 2026-09-11 新口径复核] n1_v（objdump -dr + reloc 归一）verdict=AE，13/14；
//   旧判词 DIFF 系 `game_func_report.py --no-compile`（objdump 无 -r）的假残差
//   （返回类型 bool vs ORIG unsigned char 的 .h 项另报 P7HeaderAxis）。
unsigned char CUserCharacInfo::GetCurCharacSuperState()
{
    return m_selected ? m_selected->m_superState : 0;
}