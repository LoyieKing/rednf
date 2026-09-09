// ============================================================================
// df_game_r 还原 —— APSystem::CUserProc（G2-4 批次，活动点/奖励动作处理）。
// 依据 docs/class_func_reports/APSystem__CUserProc.md + ORIG 反汇编。
// 多数方法 ORIG 即为空实现（奖励/广播在目标构建中禁用）；这里按 ORIG
// 语义转录，保留真实调用与消息构造。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CUserProc.h"   // APSystem::CUserProc / _ActionGroupIndex（唯一声明点）
#include "CUser.h"       // CUser 权威定义（m_actionPoint @+0x8d264）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

// [b28c-4] ORIG 0x081224ac 载荷为 5 字节紧凑槽（flag@-0x19、itemIdx@-0x18）
// ⇒ pack(1) 结构体（探针 /tmp/l4/reward.cpp g2 与 ORIG 逐条全等）。
struct UserProcRewardPayload
{
    char flag;
    int itemIdx;
} __attribute__((packed));

void APSystem::CUserProc::ClearActionAndSendtoUser(CUser* user,
                                                   _ActionGroupIndex group,
                                                   int value,
                                                   ENUM_PACKETCLASS packetClass)
{
    // [b28c-4] ORIG 0x08122390：空体保留 sub $0x10 帧 ⇒ 原码含未使用局部。
    // 4.4 实测对未使用局部同样占槽（/tmp/l4/frame.cpp f1/f3），照抄帧。
    char framePad[0x10];
    (void)framePad;
}

void APSystem::CUserProc::SendActionClear(CUser& user, unsigned short actionNo,
                                          _ActionGroupIndex group, bool flag,
                                          ENUM_PACKETCLASS packetClass)
{
    _MSG_ACTION_POINT_ACTION_CLEAR msg;
    // [b28c-4] ORIG 0x081223b4 赋值序 = m_actionNo → m_group → m_flag（直接字节存，
    // P136）；三元 `flag?1:0` 多出 cmp/je 常量岛（+4 条）。
    msg.m_actionNo = actionNo;
    msg.m_group = (int)group;
    msg.m_flag = flag;

    PacketGuard guard;
    if (packetClass == ENUM_PACKETCLASS_0)
    {
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x1f3);
    }
    else
    {
        ((InterfacePacketBuf*)&guard)->put_header(1, 0x228);
    }
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&msg, 0xb);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}

void APSystem::CUserProc::SendActionListState(CUser* user)
{
    // [b28c-4 P223] ORIG 0x08122398：if-body 尾 `jmp leave;nop` 空块中继岛，
    // 4.4 对 if{body} 恒 jump-thread（少 jmp+nop）⇒ goto 尾空标签复现。
    if (user == 0)
        goto L;
    SendActionListState(*user);
L:;
}

void APSystem::CUserProc::SendActionListState(CUser& user)
{
    // [b28c-4] ORIG 0x081224a0：空体 sub $0xd20 帧 ⇒ 未使用局部 char[0xd20]。
    char framePad[0xd20];
    (void)framePad;
}

// [b28d-solo-CUserProc N-G] ORIG 0x081224ac：52 条与重建全等（含 EH landing / 帧 0x40 /
// 槽 -0x19/-0x18/-0x2c），p167 8/8 CLEAR；唯一残差 = put_binary 实参装载序：ORIG(4.1)
// 先 `lea -0x19(%ebp),%eax` 再 `movl $0x5,0x8(%esp)`，4.4 在直线块内恒反（先压 arg1）。
// 该序由基本块上下文决定而非源码形：同文件 SendActionClear 的 put_binary 位于 if/else
// 汇合块首，4.4 即产 ORIG 的 lea-先形（该件 AE 实证）⇒ 源码不可控。穷举 18 形（cast /
// (void*)中转 / reinterpret_cast / &首成员 / 具名指针 / volatile 指针 / 具名 int /
// sizeof / 语句表达式 / 逗号表达式 / 地址算术 / 5u / (int)5 / 具名 receiver / volatile
// 对象 / 指针+0 / 5+0 / 宏形）全败，且 4.4.6 后端读数与 4.4.7 逐条相同（非后端敏感族，
// P230 补）⇒ 判 N-G（P75 跨版本实参装载时刻）。
void APSystem::CUserProc::SendGetRewardItem(CUser& user,
                                            _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward,
                                            bool flag)
{
    PacketGuard guard;
    ((InterfacePacketBuf*)&guard)->put_header(1, 0x229);

    UserProcRewardPayload data;
    data.itemIdx = reward.m_itemIdx;
    data.flag = flag;
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&data, 5);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}

void APSystem::CUserProc::SetTodayActionAndCheckMedalReward(CUser* user)
{
}

void APSystem::CUserProc::UpdateActionPoint(CUser* user)
{
}

void APSystem::CUserProc::GetTodayRewardAndSendPostal(
    CUser* user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
    // [b28c-4] ORIG 0x0812255c：空体保留 sub $0x10 帧（未使用局部）。
    char framePad[0x10];
    (void)framePad;
}

void APSystem::CUserProc::CheckAndSendToMessageAllUser(unsigned int a,
                                                       unsigned int b,
                                                       const char* msg)
{
    // [b28c-4] ORIG 0x08122564：空体保留 sub $0x20 帧（未使用局部）。
    char framePad[0x20];
    (void)framePad;
}

void APSystem::CUserProc::SendToMessageAllUser(const char* msg, const char* fmt)
{
    // [b28c-4 P223/BM8] ORIG 0x08122572：两条件各带尾 nop 岛（L1 为 `nop;jmp leave`、
    // L2 为 `nop` fall-through）⇒ 双 goto 尾标签串复现（探针 /tmp/l4/islands.cpp fB）。
    if (msg == 0)
        goto L1;
    if (fmt == 0)
        goto L2;
    {
        char buf[1024];
        sprintf(buf, fmt, msg);
        // [b28c-4] ORIG `movl $str,-0x18`：未使用指针须"声明后赋值"——初始化形被
        // 4.4 折叠为 movl $0（探针 /tmp/l4/lit2.cpp h1）。
        const char* pMsgName;
        pMsgName = "chn_game_server_msg_82";
        int msgLen = strlen(buf);
        int a = 0;
        int b = 0;
    }
L1:;
L2:;
}

void APSystem::CUserProc::GM_Reset(CUser* user)
{
    // [b28c-4 P223] ORIG 0x081225d2：尾 `jmp leave;nop` 岛（同 SendActionListState）。
    if (user == 0)
        goto L;
    SetTodayActionAndCheckMedalReward(user);
L:;
}

void APSystem::CUserProc::GM_Set(CUser* user, unsigned int value)
{
    // [b28c-4 P223] ORIG 0x081225ee：尾 `jmp leave;nop` 岛。
    if (user == 0)
        goto L;
    // P0：ORIG `lea 0x8d264(%eax),%edx` 单步出参无槽——禁 mgr 具名局部
    user->m_actionPoint.GM_Set(*user, value);
    SendActionListState(*user);
L:;
}
