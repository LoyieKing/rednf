// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TowerAPCLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// SendingAPC_Info::SendingAPC_Info —— ORIG T 0x085fe860 [0x085fe860,0x085fe8c9]
// 报告 SendingAPC_Info/SendingAPC_Info.md：构造 +0x04 PacketBuf → clear →
// memset(m_apcInfo@+0x17338, 0, 0xbc)。
#include "TowerAPCLeafC3/SendingAPC_Info.inc"
{
    m_buf.clear();
    memset(&m_apcInfo, 0, 0xbc);
}

// SendingAPC_Info::~SendingAPC_Info —— ORIG T 0x085fe8ca [0x085fe8ca, 0x085fe8df]
// 仅析构 +0x04 内嵌 PacketBuf（m_buf），成员自动析构等价。
#include "TowerAPCLeafC3/dtor_SendingAPC_Info.inc"
{
}

// [b28d-32 N(P75): 1 条装载序/寄存器差（202/201） | ORIG 0x085fe906]
// 【b28e-M1S2 本波复核】202/201。P331 核过：残差为**多段不同常数**平移
//   （strlen 结果槽 ORIG -0x20 vs 我方 -0x28（差 8）；6 个 int 局部组
//   ORIG -0x50..-0x3c vs 我方 -0x24..-0x10（差 0x2c））⇒ 非「恒定平移」⇒ 封 N。
//   %edi 保活差：ORIG `lea 0x4(%eax),%edi`（this+4 跨 cMyTrace 调用存活）vs
//   我方 %esi；帧 0x7c vs 0x70（= ORIG 多 push %edi + 槽深差）。
//   语义轴：cMyTrace 格式串 `@TOD : setting layer:%d, name:%s, lv:%d, job:%d,
//   grow_type:%d, guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d`
//   与 ORIG 0x8cd6904 逐字节一致（objdump -s 核对），9 个实参装载段
//   （0x28..0x10(%esp)）与 ORIG 逐条对应。
#include "TowerAPCLeafC3/Set_APC_Info.inc"
{
    // b21 verdict=DIFF（N 残余）：202/201（gap=1）。调用点全等（clear/put_header/put_byte×/
    //   GetLayer/strlen×3/put_int×/put_str×3/finalize/cMyTraceC1+cMyTracecl）。包字段序
    //   （userCharacNo/name/lv/job/grow/guild/pvp/abilities[0x16]/creature/creatureId）逐位核。
    //   trace 实参物化两批局部槽且顺序差(N82)、GetLayer 结果寄存器、帧 push edi vs 栈。
    //   语义完成，byte 层 N 封顶（近 identical）。
    // [N3 复核 2026-09-11] 两旋钮实测均不过闸门（基线 our=201/gap=+1）：
    // ① 临时对象直接调用形 `cMyTrace(...)(...)`（src/TowerAPCLeafC3.cpp 由具名 trace 改造）
    //    ⇒ ORIG 的 ctor 发射点（0x085feb00 实参物化**之后**）与 prologue `push %edi` 复现，
    //    但 our 201→203（gap +1→−1）、残差转为全帧槽映射差（trace 实参槽 ORIG
    //    -0x50/-0x4c/-0x48/-0x44/-0x40/-0x3c、ctor -0x38；我方 -0x40/-0x3c/-0x38/-0x34…、ctor -0x50）
    //    ⇒ 判负例。
    // ② P339 全头部声明（6 个 trace 实参局部提头、声明序=ORIG 槽深→浅）叠加① ⇒ our 203 不变、
    //    槽仅整体深移 0x10 仍不对齐 ⇒ 判负例。
    //    ⇒ 保留原形（具名 trace + 块内声明），按 N82/N95 槽族封顶。
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

    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x68, 5);
    trace("@TOD : setting layer:%d, name:%s, lv:%d, job:%d, grow_type:%d, "
          "guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d",
          (unsigned int)(layer.GetLayer() & 0xffff),
          info.m_userCharacName, lv, job, growType,
          guildName, pvpGrade, creatureName,
          info.m_creatureId);
}
// ---------------------------------------------------------------------------
#include "TowerAPCLeafC3/Send_APC_Info.inc"
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
#include "TowerAPCLeafC3/SendMessageToCUser.inc"
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
// [b28d-32 N(P75): 装载序/寄存器差（90/87） | ORIG 0x085feee4]
#include "TowerAPCLeafC3/Send_APC_Info_1.inc"
{
    // ORIG 形态：默认构造 it → end() 先求值 → find 赋值 → operator!= 判断；
    // 日志三参为内联 getter 表达式（右到左求值，寄存器临时跨 noop 构造存活）。
    // [b28e-M1S2] 反转声明序（endIt 先）实测 verdict NEAR→DIFF（90/90 不变，槽映射
    // 未如预期互换且 end() 发射点提前）⇒ 回滚。残留 = 两个迭代器槽互换：
    // ORIG it=-0x34/endIt=-0x30（4.1 先声明深），我方 it=-0x30/endIt=-0x34
    //（4.4 先声明浅，与本件实测一致）⇒ P339 方向相反、无可造形，判 N(P75 槽序)。
    // [P6AuditOldN-S1 2026-09-11 负结论] P339 第三形「声明序 endIt→it + 赋值分离」实测
    // 反劣：our 90→95（gap 0→−5），多出 endIt 默认构造 call + 追加拷贝
    //（`lea -0x30; call it-ctor` ⇒ `mov -0x3c,%eax; mov %eax,-0x30`）。
    // 4.1/4.4 槽方向相反在此件无源码可造形（带初始化器反转亦为负例，前波已证）⇒ 封 N。
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator endIt = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != endIt)
    {
        it->second.Send_APC_Info(layer, user);
        // [b28e-M1S2] ORIG 0x085ff2xx：noop 的 ctor 在三个 getter 求值**之后**
        // （`call get_user_charac_no; mov %eax,%esi; call GetLayer; movzwl %ax,%edi;
        //  call getCurCharacNo; mov %eax,%ebx; lea -0x29(%ebp),%eax;
        //  call cMyTraceNoop::C1Ev`）⇒ 源码为**临时对象直接调用**形（this 实参
        // 最后求值），非具名 `cMyTraceNoop noop; noop(...)`（4.4 会在声明处提前
        // 构造 ctor）。同因改写 else 支的 cMyTrace。
        cMyTraceNoop()("TOD:sending [%d] toUser, layer [%d] : clone : [%d]",
                       user->getCurCharacNo(),
                       layer.GetLayer(),
                       it->second.get_user_charac_no());
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa1, 5)(
            "TOD:invalid layer %d", (unsigned int)(layer.GetLayer() & 0xffff));
    }
}
