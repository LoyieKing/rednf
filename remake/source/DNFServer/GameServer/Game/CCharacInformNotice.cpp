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
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   checkInformNoticeFlag 0x080e4892 DIFF(33/28) N96+N18a+P168a④：ORIG
//     m_flags 落槽→sar/and→xor$1/test 常量岛；4.4 对 sar/and 后 eax 恒以
//     movzbl 加宽顶掉 xor 段，无绑定不可复现（清绑后自然形，P0 合规）⇒ 封顶。
//   handleInformNoticeFlag 0x080e4806 DIFF(55/54) 修法：P223 多岛形（三失败
//     出口各落独立 nop 岛，负条件 goto 连续尾标签，探针 fA 逐条复现三岛）；
//     残差 = N18a 两段物化 ×2 + 第三守卫 4.4 `xor$1;test;jne` vs ORIG `test;je`
//     （P168a①/P292 bool 归一，无绑定不可造）⇒ 封顶。P167 CLEAR(2/2)。
// ============================================================================


// ORIG C1 0x080e4790：基类 CData ctor（隐式）后 _reset。
CCharacInformNotice::CCharacInformNotice()
{
    _reset();
}

CCharacInformNotice::~CCharacInformNotice()
{
}

// ORIG T 0x080e4ab8：memset(this+5, 0, 0x10)
void CCharacInformNotice::_reset()
{
    memset(m_flags, 0, sizeof(m_flags));
}

// ORIG T 0x080e4892：bit>7 或 byte>0xf → 0；否则返回 !(flag bit set)
bool CCharacInformNotice::checkInformNoticeFlag(unsigned int bit, unsigned int byteIdx)
{
    if (bit > 7)
        return false;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return false;
    // [b28c-L15 P0/N96] 存量 register asm("eax") 绑定已清除（全域禁绑，N96）。
    //   ORIG 0x080e4ab8 形：m_flags[byteIdx] 落槽 → sar/and → `xor $0x1,%eax;
    //   test %al,%al` + mov$1/mov$0 常量岛。P168a④：int 表达式任何源形均仍拆两段
    //   （N18a），且 4.4 对 sar/and 后的 eax 恒以 movzbl 加宽顶掉 xor 段
    //   （P168a③ 总闸），无绑定即不可复现 ⇒ 落自然形，N96+N18a+P168a④ 封顶。
    //   ①X2 无绑定形（uchar 局部 ^= 1 + if）实测恒三段栈槽物化（`mov %al,-0x5;
    //     xorb $1,-0x5; cmpb $0,-0x5`），命中 P168a④ ⇒ 维持 N；
    //   ②裸 register（无 asm 绑定，采纳 lane22/23 实测：可免槽、且能复现 4.1 的
    //     bool 归一常量岛）本点位实测劣于③——4.4 把 t 钉到 %ebx 并多插一条
    //     `mov %eax,%ebx`（ORIG 值已在 %eax，`and/xor $1,%eax; test %al` 零中转），
    //     AE 归一后差 7 行 vs ③ 的 5 行 ⇒ 不采纳；
    //   ③本行：裸自然形（P168a①）——sar/and 后取反直返，残 5 条 = 4.1 bool 归一岛。
    unsigned char v = m_flags[byteIdx];
    return !(((int)v) >> bit & 1);
}

// ORIG T 0x080e4806：flag&7=bit，flag>>3=byte；check 真则置位并 alter。
// b28d-55 P223 多岛形：ORIG 三失败出口各落独立 nop 岛（+0x7d nop;jmp / +0x80
// nop;jmp / +0x83 nop，标签序=守卫序）⇒ 负条件 goto 到连续尾标签（探针
// /tmp/b28d_l55/probe_p223.cpp fA 逐条复现三岛）。byteIdx 含初始化须包内层复合块
// （P223 扩展：4.4 拒 goto 跨同层非平凡初始化声明）。
void CCharacInformNotice::handleInformNoticeFlag(unsigned int flag)
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
    // 残差 N：①ORIG movzbl→edx→ebx / lea→edx→ecx 两段物化为 N18a 反向折叠
    //（4.4 恒单段不可造）；②第三守卫 ORIG `test %al,%al; je` vs 4.4 `xor $0x1;
    //  test %al,%al; jne`（P168a①/P292 bool 归一，无绑定不可造）。
}

// ORIG T 0x080e48ec：movzbl 0x5(%edx,%eax,1),%eax 直返（P9：返回 unsigned char，单条零扩展）。
unsigned char CCharacInformNotice::getInformNoticeFlag(int idx)
{
    return m_flags[idx];
}

// ORIG T 0x080e48fc：位展开为索引清单（clear + 双层 setle 循环 + push_back）。
void CCharacInformNotice::getInformNoticeFlagIndex(std::vector<unsigned char>& out)
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
void CCharacInformNotice::sendInformNoticeFlag(CUser* user)
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
bool CCharacInformNotice::loadData(CUser* user, char* data)
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
bool CCharacInformNotice::_saveData(CUser* user)
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

void CCharacInformNotice::getData(char* data) const
{
    memcpy(data, m_flags, sizeof(m_flags));
}
