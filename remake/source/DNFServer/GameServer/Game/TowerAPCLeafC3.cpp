// ============================================================================
// TowerAPCLeafC3.cpp —— TowerOfDespair APC 发送链（TowerAPCLeafC3 批次）
//
// 逐指令依据 docs/class_func_reports/（## 汇编 + ## 反编译 C 已完整读取）：
//   TowerOfDespair_APC_Mgr/Send_APC_Info.md   ORIG T 0x085feee4（本批次目标）
//   SendingAPC_Info/Send_APC_Info.md          ORIG T 0x085febca
//   SendingAPC_Info/Set_APC_Info.md           ORIG T 0x085fe906
//   TowerOfDespairMgr/SendMessageToCUser.md   ORIG T 0x08644366（静态）
// 辅助 W 内联符号与 ORIG nm 一致：
//   TOD_Layer::GetLayer            0x0822ec40 W（头文件 inline）
//   TOD_Layer::operator<           0x085ff114 W（头文件 inline；map key 序）
//   SendingAPC_Info::get_user_charac_no      0x085ff12c W（头文件 inline）
//   cMyTraceNoop C1/cl             0x0822ad5a/0x0822ad60 W（既有独立 TU 提供）
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "TowerOfDespairMgr.h"
#include "GameTypes.h"       // cMyTrace(char const*, int, int) / operator()(char const*, ...)
#include "cMyTraceNoop.h"    // cMyTraceNoop
#include "PacketGuard.h"
#include "CUser.h"           // Send(PacketGuard&) / isGMUser / getCurCharacNo

// ---------------------------------------------------------------------------
// SendingAPC_Info::Set_APC_Info —— ORIG T 0x085fe906 [0x085fe906, 0x085febc9]
// clear → m_userCharacNo=info[+0] → put_header(0,0x15a) → put_byte(layer)
// → (put_int(len)+put_str(name@+4)) → put_byte(lv/job/grow_type @+24/+28/+2c)
// → (put_int(len)+put_str(guild@+30)) → put_int(pvp_grade@+48)
// → 循环 i=0..21：put_int(*(int*)(info + 0x4c + i*4))（能力表 22 项）
// → (put_int(len)+put_str(creature_name@+a4)) → put_int(creature_id@+b4)
// → finalize(true) → cMyTrace 日志。
// ---------------------------------------------------------------------------
// SendingAPC_Info::~SendingAPC_Info —— ORIG T 0x085fe8ca [0x085fe8ca, 0x085fe8df]
// 仅析构 +0x04 内嵌 PacketBuf（m_buf），成员自动析构等价。
SendingAPC_Info::~SendingAPC_Info()
{
}

void SendingAPC_Info::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    m_buf.clear();
    m_userCharacNo = info.m_userCharacNo;
    m_buf.put_header(0, 0x15a);
    m_buf.put_byte(layer.GetLayer());

    int nameLen = (int)strlen(info.m_userCharacName);
    m_buf.put_int(nameLen);
    m_buf.put_str(info.m_userCharacName, nameLen);

    m_buf.put_byte(info.m_lv);
    m_buf.put_byte(info.m_job);
    m_buf.put_byte(info.m_growType);

    int guildLen = (int)strlen(info.m_guildName);
    m_buf.put_int(guildLen);
    m_buf.put_str(info.m_guildName, guildLen);

    m_buf.put_int(info.m_pvpGrade);
    for (int i = 0; i < 0x16; ++i)                  // ORIG cmpl $0x15/setle
        m_buf.put_int(info.m_abilities[i]);

    int creatureLen = (int)strlen(info.m_creatureName);
    m_buf.put_int(creatureLen);
    m_buf.put_str(info.m_creatureName, creatureLen);

    m_buf.put_int(info.m_creatureId);
    m_buf.finalize(true);

    // ORIG +0x1fb..+0x2b1：trace 实参先物化到局部槽（creature_id→esi、
    // &creature_name/-0x50、pvp/-0x4c、&guild/-0x48、grow/-0x44、job/-0x40、
    // lv/-0x3c、GetLayer→ebx），再反序写入变参栈槽。
    char* creatureName = const_cast<char*>(info.m_creatureName);
    int pvpGrade = info.m_pvpGrade;
    char* guildName = const_cast<char*>(info.m_guildName);
    int growType = info.m_growType;
    int job = info.m_job;
    int lv = info.m_lv;

    cMyTrace trace("void SendingAPC_Info::Set_APC_Info(const TOD_Layer&, const _APCInfo&)",
                   0x68, 5);
    trace("@TOD : setting layer:%d, name:%s, lv:%d, job:%d, grow_type:%d, "
          "guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d",
          (unsigned int)(layer.GetLayer() & 0xffff),
          info.m_userCharacName, lv, job, growType,
          guildName, pvpGrade, creatureName,
          info.m_creatureId);
}
// ---------------------------------------------------------------------------
void SendingAPC_Info::Send_APC_Info(TOD_Layer const& layer, CUser* user)
{
    Set_APC_Info(layer, m_apcInfo);

    PacketGuard guard(&m_buf);
    user->Send(guard);

    char msg[256];
    sprintf(msg,
            "TOD : setting layer:%d, name:%s, lv:%d, job:%d, grow_type:%d, "
            "guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d",
            (unsigned int)(layer.GetLayer() & 0xffff),
            m_apcInfo.m_userCharacName, m_apcInfo.m_lv, m_apcInfo.m_job,
            m_apcInfo.m_growType, m_apcInfo.m_guildName, m_apcInfo.m_pvpGrade,
            m_apcInfo.m_creatureName, m_apcInfo.m_creatureId);
    TowerOfDespairMgr::SendMessageToCUser(user, msg);
}

// ---------------------------------------------------------------------------
// TowerOfDespairMgr::SendMessageToCUser —— ORIG T 0x08644366
//                                        [0x08644366, 0x0864446f]（静态成员，
//                                        asm 无 this 槽，user@+8/msg@+0xc）
// isGMUser 命中才发包：PacketGuard() → put_header(0,0xc) → byte/short/byte 0
// → put_int(strlen(msg)) → put_str(msg, strlen(msg))（ORIG 调用两次 strlen）
// → finalize(true) → Send。
// ---------------------------------------------------------------------------
void TowerOfDespairMgr::SendMessageToCUser(CUser* user, char* msg)
{
    if (user->isGMUser())
    {
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int((int)strlen(msg));
        guard.put_str(msg, (int)strlen(msg));
        guard.finalize(true);
        user->Send(guard);
    }
}

// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Send_APC_Info —— ORIG T 0x085feee4
//                                          [0x085feee4, 0x085ff01f]（本批次目标）
// find(layer)；未命中 → cMyTrace("...Send_APC_Info(const TOD_Layer&, CUser*)",
// 0xa1, 5)("TOD:invalid layer %d", GetLayer)；
// 命中 → it->second.Send_APC_Info(layer,user) → get_user_charac_no()
// → cMyTraceNoop 构造后输出
// "TOD:sending [%d] toUser, layer [%d] : clone : [%d]"(getCurCharacNo,layer,no)。
// 注：运行期 mgr 对象由 CtorProducers_1.cpp 的 new 创建（空 map 内存映像与
// 真实 map<TOD_Layer,SendingAPC_Info> 一致）；填充方 Set_APC_Info 后续批次
// 接入时须按本真实容器模型实现。
// ---------------------------------------------------------------------------
void TowerOfDespair_APC_Mgr::Send_APC_Info(TOD_Layer const& layer, CUser* user)
{
    // ORIG 形态：默认构造 it → end() 先求值 → find 赋值 → operator!= 判断；
    // 日志三参为内联 getter 表达式（右到左求值，寄存器临时跨 noop 构造存活）。
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator endIt = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != endIt)
    {
        it->second.Send_APC_Info(layer, user);
        cMyTraceNoop noop;
        noop("TOD:sending [%d] toUser, layer [%d] : clone : [%d]",
             user->getCurCharacNo(),
             layer.GetLayer(),
             it->second.get_user_charac_no());
    }
    else
    {
        cMyTrace trace("void TowerOfDespair_APC_Mgr::Send_APC_Info(const TOD_Layer&, CUser*)",
                       0xa1, 5);
        trace("TOD:invalid layer %d", (unsigned int)(layer.GetLayer() & 0xffff));
    }
}
