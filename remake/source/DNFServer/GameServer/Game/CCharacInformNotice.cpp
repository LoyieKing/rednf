// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCharacInformNotice/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// ORIG 域常量（nm：_ZN12InformNoticeL18kInformNoticeIndexE / L16kInformNoticeBitE，
// r 段内部链接；m_flags[16] × 8bit 标志板）
namespace InformNotice
{
static const int kInformNoticeIndex = 16;
static const int kInformNoticeBit = 8;
}
#include "CCharacInformNotice.h"
#include "CUser.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include <string.h>

// ============================================================================
// [b28d-solo-CCharacInformNotice] 本 TU 判词表（verdict = check_tu_routed.sh fresh .o）：
//   checkInformNoticeFlag 0x080e4892 FIXED→AE（33=33）：P56 if-return 形复现 ORIG
//     尾部 `xor$1; test%al; je; mov$1; jmp; mov$0` 常量岛；.o 残余仅未重定位分支
//     目标显示（P264）。
//   handleInformNoticeFlag 0x080e4806 DIFF(55/54) N 封顶：P223 三岛形（`!f()` 负
//     条件 goto 连续尾标签 L1/L2/L3，逐岛复现）；残差①第三守卫 4.4
//     `xor$1;test%al;jne` vs ORIG `test%al;je`（P168a①/P292 bool 归一不可造；
//     正向 if 可消 xor 但 4.4 把 if-false 落点前置并 jump-thread 岛2/岛3，51<55 更劣）；
//     ②两处 N18a 反向折叠（ORIG movzbl→edx→ebx、lea→edx→ecx 中转，4.4 恒单段）。
//     P248 三档试尽：m_flags 元素改 1 字节 enum 仅翻第一处中转（/tmp/cn_probe4.cpp
//     v10/v13/v15），第二处与 xor 在 12 形下均不可翻（/tmp/cn_probe5..9.cpp）；
//     4.4.6 后端复测同形（非 N7 后端敏感族）。P167 CLEAR(2/2)。
// ============================================================================


// ORIG C1 0x080e4790：基类 CData ctor（隐式）后 _reset。
#include "CCharacInformNotice/CCharacInformNotice.inc"
{
    _reset();
}

#include "CCharacInformNotice/dtor_CCharacInformNotice.inc"
{
}

// ORIG T 0x080e4ab8：memset(this+5, 0, 0x10)
#include "CCharacInformNotice/_reset.inc"
{
    memset(m_flags, 0, sizeof(m_flags));
}

// ORIG T 0x080e4892：bit>7 或 byte>0xf → 0；否则返回 !(flag bit set)
// [b28d-solo-CCharacInformNotice] 判词 AE(33=33)：P56 if-return 形修复——ORIG 尾部为
//   `xor$1,%eax; test%al,%al; je; mov$1; jmp; mov$0`（bool 归一 if-return 岛），
//   原先 `return !expr;` 只发 xor 直返（28 条）⇒ 改 `if(!expr) return true;
//   return false;` 后逐条对齐（.o 残余仅重定位分支目标显示，P264）。
#include "CCharacInformNotice/checkInformNoticeFlag.inc"
{
    if (bit > 7)
        return false;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return false;
    // [b28c-L15 P0/N96] 存量 register asm("eax") 绑定已清除（全域禁绑，N96）。
    unsigned char v = m_flags[byteIdx];
    if (!(((int)v) >> bit & 1))
        return true;
    return false;
}

// ORIG T 0x080e4806：flag&7=bit，flag>>3=byte；check 真则置位并 alter。
// [b28d-solo-CCharacInformNotice] 判词 DIFF(55/54) N 封顶：P223 多岛形——ORIG 三失败
//   出口各落独立 nop 岛（+0x7d nop;jmp / +0x80 nop;jmp / +0x83 nop，标签序=守卫序），
//   负条件 goto 到连续尾标签 L1/L2/L3 逐岛复现。byteIdx 含初始化须包内层复合块
//   （P223 扩展：4.4 拒 goto 跨同层非平凡初始化声明）。
//   残差 N：①第三守卫 ORIG `call; test%al,%al; je` vs 4.4 `call; xor$1; test%al,%al;
//   jne`（P168a①/P292 bool 归一恒物化；`==0`/`==false`/`!f()`/`return` 同形；正向
//   if 可消 xor 但 4.4 前置 if-false 落点并 jump-thread 岛2/岛3 → 51<55 更劣）；
//   ②两处 N18a 反向折叠：ORIG `movzbl 0x5(%ecx,%edx,1),%edx; mov %edx,%ebx` 与
//   `lea (%ebx,%edx,1),%edx; mov %edx,%ecx`，4.4 恒单段直落。
//   P248 三档试尽（探针 /tmp/cn_probe*.cpp）：m_flags 元素 1 字节 enum 化翻正第一处
//   中转（v10/v13/v15），第二处中转与 xor 在 1u/1/+=/具名/register/uchar 截断/enum
//   RHS 共 12 形下均不可翻；4.4.6 后端复测同形（非 N7 后端敏感族）。
#include "CCharacInformNotice/handleInformNoticeFlag.inc"
{
    unsigned int bit = flag & 7;
    if (bit > 7)
        goto L1;
    {
        unsigned int byteIdx = flag >> 3;
        if (byteIdx >= InformNotice::kInformNoticeIndex)
            goto L2;
        if (!checkInformNoticeFlag(bit, byteIdx))
            goto L3;
        m_flags[byteIdx] = (unsigned char)(m_flags[byteIdx] + (1u << bit));
        alter();
        goto END;
    }
L1:;
L2:;
L3:;
END:;
}

// ORIG T 0x080e48ec：movzbl 0x5(%edx,%eax,1),%eax 直返（P9：返回 unsigned char，单条零扩展）。
#include "CCharacInformNotice/getInformNoticeFlag.inc"
{
    return m_flags[idx];
}

// ORIG T 0x080e48fc：位展开为索引清单（clear + 双层 setle 循环 + push_back）。
#include "CCharacInformNotice/getInformNoticeFlagIndex.inc"
{
    out.clear();
    for (int byteIdx = 0; byteIdx < InformNotice::kInformNoticeIndex; ++byteIdx)
    {
        unsigned char bits = static_cast<unsigned char>(getInformNoticeFlag(byteIdx));
        for (int bitIdx = 0; bitIdx < InformNotice::kInformNoticeBit; ++bitIdx)
        {
            if (((bits >> bitIdx) & 1) != 0)
            {
                out.push_back(static_cast<unsigned char>(
                    byteIdx * InformNotice::kInformNoticeBit + bitIdx));
            }
        }
    }
}

// ORIG T 0x080e4994：vector 索引清单 + 0x1b8 包（put_header/put_byte/finalize + CUser::Send）。
#include "CCharacInformNotice/sendInformNoticeFlag.inc"
{
    std::vector<unsigned char> indexes;
    getInformNoticeFlagIndex(indexes);

    PacketGuard packet;
    packet.put_header(0, 0x1b8);
    packet.put_byte(static_cast<int>(indexes.size()));
    for (size_t i = 0; i < indexes.size(); ++i)
    {
        packet.put_byte(static_cast<int>(indexes[i]));
    }
    packet.finalize(true);
    user->Send(packet);
}

// ORIG T 0x080e4ade：vtable+0x1c 虚调用 _reset → memcpy(this+5,data,0x10) → sendInformNoticeFlag → 1。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "CCharacInformNotice/loadData.inc"
{
    _reset();
    memcpy(m_flags, data, sizeof(m_flags));
    sendInformNoticeFlag(user);
    return true;
}

// [b22判词·N-codegen] _saveData 语义逐条忠实 ORIG 0x080e4b52（报告汇编+Ghidra C 全读）：
// Acquire(s_stream_pool,"CharacInformNotice.cpp",0x77) → CStreamGuard(,true) →
// operator*()->operator<<(0x311) → operator*()->operator<<(GetUID)（GetUID 先求值）→
// operator->()->GetInBuffer<SIG> → 空则返 false → memset(sig,0,0x14) →
// sig->m_characNo=get_charac_no(-1) → memcpy(sig+4,this+5,0x10) → s_msgq_mgr->put(DB_Q=2,guard)
// → 返 true（含 CStreamGuard EH 析构岛 + _Unwind_Resume）。orig87/our86 唯一真差：
// `*sig=get_charac_no` 存储序列 ORIG 走 `mov %eax,%edx; mov sig,%eax; mov %edx,(%eax)`
// （结果先让位 %edx 再载 sig，4 条），4.4.7 -O0 走 `mov sig,%edx; mov %eax,(%edx)`
// （结果留 %eax，3 条）——跨版本寄存器分配次序差，源码层无法稳定复现（试过结果具名
// 临时量：4.4 落槽反多 2 条，已回退）。其余 diff（call 目标/分支偏移/数据地址
// 0x940bd6c、0x8b2dd20、s_msgq_mgr）全属未 relink .o 重定位占位，relink 后转 AE。语义等价。
#include "CCharacInformNotice/_saveData.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacInformNotice.cpp", 0x77), true);
    // ORIG（idx51 报告）：每次 << 先 operator*()（de）取流再 operator<<；GetUID
    // 先求值（ORIG +0x057 call GetUID → +0x064 operator* → +0x076 operator<<）。
    guard.operator*()->operator<<(0x311);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        guard->GetInBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x14);
    sig->m_characNo = user->get_charac_no(-1);
    memcpy(sig->m_flags, m_flags, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

#include "CCharacInformNotice/getData.inc"
{
    memcpy(data, m_flags, sizeof(m_flags));
}
