// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LC_Dispatchers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：Dispatcher_MouseRegister 三方法 +
// Dispatcher_RegisterToBlackList::process。
// ORIG 依据（docs/class_func_reports/）：
//   Dispatcher_MouseRegister/process.md  T 0x08263224
//   Dispatcher_MouseRegister/read.md     T 0x08263210
//   Dispatcher_MouseRegister/send.md     T 0x082632b4
//   Dispatcher_RegisterToBlackList/process.md T 0x081cdc9a
// 附加依赖 producer：Packet_Register_To_BlackList C1 W 0x0822bb2e
//   （objdump 逐指令：PacketHeader(0x5dc,0x2c) + memset(this+0xe,0,0x1e)）。
#include "Dispatchers.h"

#include <cstring>

#include "CEnvironment.h"       // G_CEnvironment / get_server_group
#include "CGameManager.h"
#include "CUser.h"              // get_acc_id / GetServerGroup
#include "CompoundEmblemDeps.h" // LineFunc 权威声明
#include "GlobalData.h"         // s_monitor_proxy_mgr / s_guild_proxy_mgr
#include "CMonitorServerProxy.h"
#include "PacketHeader.h"      // 权威 PacketHeader（0xa packed）
#include "CServerProxyMgr.h"

// ---------------------------------------------------------------------------
// Packet_Register_To_BlackList（0x2c；ctor W 0x0822bb2e；本 TU 唯一定义点）。
// +0x00 PacketHeader(0xa packed) / +0x0a accId / +0x0e char name[0x1e]。
class Packet_Register_To_BlackList
{
public:
    Packet_Register_To_BlackList()
        : m_header(0x5dc, 0x2c)
    {
        memset(m_name, 0, sizeof(m_name));
    }

    PacketHeader m_header;    // +0x00
    unsigned int m_accId;     // +0x0a
    char m_name[0x1e];        // +0x0e
} __attribute__((packed));

// ---------------------------------------------------------------------------
// ORIG 0x08263224（报告 Dispatcher_MouseRegister/process.md）：
// param.m_4 = check_error(user,msg)；>0 直返；<0 → return LineFunc(0x178,...)；
// ==0 路径物化 msg 死槽（ORIG 尾 mov 0x10(%ebp)=&msg；4.1 死局部发射，
// 探针 /tmp/p17c.cpp procA 同形）。块排布与 ORIG 逐条同形。
// （check_error 定义见 Dispatchers.cpp）
#include "LC_Dispatchers/process.inc"
{
    TempParam& out = static_cast<TempParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x178,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    MSG_BASE* dead = &msg;
    (void)dead;
    return 0;
}

// ORIG 0x08263210：恒返回 0。
#include "LC_Dispatchers/read.inc"
{
    // ORIG 0x08263210：`sub $0x10; mov 0x10(%ebp),%eax; mov %eax,-0x4(%ebp);
    // mov $0,%eax` ——第二引用实参在 4.1 入口物化到局部槽（p=&msg 形，P0 槽
    // 对应律；4.4 直读形参槽会消掉这三条）。packet 无槽（ORIG 不物化第一参）。
    MSG_BASE* p = &msg;
    (void)p;
    return 0;
}

// ORIG 0x082632b4：读取 param+0x04（ORIG 尾部 eax 返回值；头声明 void，
// dispatch 模板不消费返回值）。
#include "LC_Dispatchers/send.inc"
{
    (void)user;
    TempParam& out = static_cast<TempParam&>(param);
    if (out.m_4) return;  // ORIG 尾 load+test（探针 senJ 同形）
}

// ---------------------------------------------------------------------------
// ORIG 0x081cdc9a（报告 Dispatcher_RegisterToBlackList/process.md）：
// check_error → param+4；>0 返回 0；>=0 时组包转发 Monitor/Guild 代理；
// 韩服（serverGroup==3）且名单命中两个 GM 名串（EUC-KR，0x1d 字节逐字节比较）
// → param+4 = 0x7fffffff；<0 → LineFunc(0x269f,...)。
// [b28d-D33 N7/N109 封顶] ORIG 0x081cdc9a：115 vs 111 条；ORIG push ebx +
// sub 0x54 vs 4.4 sub 0x58（callee-saved 需求差，N7/N109）。p167 CLEAR 15=15。
#include "LC_Dispatchers/process_1.inc"
{
    stRegisterToBlackListParam& out = static_cast<stRegisterToBlackListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
        return 0;
    if (out.m_4 >= 0)
    {
        Packet_Register_To_BlackList packet;
        packet.m_accId = user->get_acc_id();

        // strncpy(msg+0x14, len=msg+0x10)（MSG_REGISTER_TO_BLACKLIST 名字字段）
        MSG_REGISTER_TO_BLACKLIST& req =
            reinterpret_cast<MSG_REGISTER_TO_BLACKLIST&>(msg);

        // MSG_REGISTER_TO_BLACKLIST 布局未还原（PacketTypes.h 占位 m_pad）：
        // ORIG 读 +0x10（u32 len）、+0x14（名字首址）——保持偏移访问并标注。
        strncpy(packet.m_name,
                reinterpret_cast<char*>(&req) + 0x14,
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&req) + 0x10));
        // ORIG 韩服判定：G_CEnvironment+0x378 == 3（serverGroup==3）才做 GM 名单比较。
        // ORIG 直接读 m_serverEnvir.m_serverGroup（+0x378 内联读取，无 get_server_group 调用）。
        if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
        {
            // DAT_08bc17d6 / DAT_08bc17dd：EUC-KR GM 名串区（0x1d 字节精确比较）
            static const char kGmName1[0x1d] = {
                '\xbb', '\xfe', '\xbf', '\xc0', '\xc0', '\xaf', '\x00',
                '\xbb', '\xfe', '\xc0', '\xaf', '\xb8', '\xae', '\x00',
                'S', 'K', 'I', 'L', 'L', '_', 'C', 'O', 'M', 'M', 'A', 'N', 'D', '_', 'L'
            };
            static const char kGmName2[0x1d] = {
                '\xbb', '\xfe', '\xc0', '\xaf', '\xb8', '\xae', '\x00',
                'S', 'K', 'I', 'L', 'L', '_', 'C', 'O', 'M', 'M', 'A', 'N', 'D', '_', 'L', 'O', 'G', ' ', ':', ' ', 'C', 'U'
            };
            if (strncmp(packet.m_name, kGmName1, 0x1d) == 0 ||
                strncmp(packet.m_name, kGmName2, 0x1d) == 0)
            {
                out.m_4 = static_cast<int>(0x7fffffff);
                return 0;
            }
        }

        // [b23S3C] P48：ORIG 两代理点各调 user->GetServerGroup() 一次（p167
        // MISSING -1 即此），无 group 槽。
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitor->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
        CGuildServerProxy* guild =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        guild->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
        return 0;
    }

    unsigned int accId = user->get_acc_id();
    LineFunc(0x269f,
             __PRETTY_FUNCTION__,
             out.m_4, accId);
    return 0;
}
