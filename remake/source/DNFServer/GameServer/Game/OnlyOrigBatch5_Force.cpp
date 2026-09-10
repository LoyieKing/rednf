// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch5_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>

#include "../../../shared/packet/include/Packet_DeathTower_Statistic_Value.h"
#include "../../../shared/packet/include/Packet_Forbid_Chat_By_Monitor.h"
#include "../../../shared/packet/include/Packet_GameServer2Statisctics2DBServer.h"
#include "../../../shared/packet/include/Packet_Guild_Check_Guild_Cargo_Push_Item.h"
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Move_Item.h"
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Pop_Item.h"
#include "../../../shared/packet/include/Packet_HellParty_Statistic_Item.h"
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"
#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"
#include "../../../shared/packet/include/Packet_Monitor_Notice_Message.h"
#include "../../../shared/packet/include/Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"
#include "../../../shared/packet/include/Packet_P2P_Statistics.h"
#include "../../../shared/packet/include/Packet_Pcroom_Response.h"
#include "../../../shared/packet/include/Packet_PvPResult.h"
#include "../../../shared/packet/include/Packet_User_Ting_TimeCheck_Statistic_Add.h"
#include "../../../shared/packet/include/Packet_Value_Statistic.h"
#include "../../../shared/packet/include/Packet_VillageMonsterFightResult.h"

#include "W8LT_PassiveObjectScript.h"  // PassiveObjGeneratorObject/Pattern/Sequence
#include "Quest.h"                     // PostalReward
#include "CDataManager.h"              // PvPSkillTreeKey
#include "RandomOptionScript.h"        // RandomApplicationKey/QuantityRatio/ResetValue/PartSelectonField
#include "SigTypes.h"                  // SIG_* 全部分片

// ============================================================================
// force 发射（匿名命名空间，防外部链接）
// ============================================================================
namespace
{

#include "OnlyOrigBatch5_Force/forceEmitBatch5.inc"
{
    // ---- Packet_* ctor（shared/packet/include 头内联，PacketHeader + 字段清零）----
    Packet_DeathTower_Statistic_Value p1;
    (void)p1;
    Packet_Forbid_Chat_By_Monitor p2;
    (void)p2;
    Packet_GameServer2Statisctics2DBServer p3;
    (void)p3;
    Packet_Guild_Check_Guild_Cargo_Push_Item p4;
    (void)p4;
    Packet_Guild_Guild_Cargo_Move_Item p5;
    (void)p5;
    Packet_Guild_Guild_Cargo_Pop_Item p6;
    (void)p6;
    Packet_HellParty_Statistic_Item p7;
    (void)p7;
    Packet_Monitor_Guild_Chat p8;
    (void)p8;
    Packet_Monitor_MegaPhone p9;
    (void)p9;
    Packet_Monitor_Member_Chat p10;
    (void)p10;
    Packet_Monitor_Notice_Message p11;
    (void)p11;
    Packet_Monitor_Other_Channel_Chat_Hyper_Link p12;
    (void)p12;
    Packet_P2P_Statistics p13;
    (void)p13;
    Packet_Pcroom_Response p14;
    (void)p14;
    Packet_PvPResult p15;
    (void)p15;
    Packet_User_Ting_TimeCheck_Statistic_Add p16;
    (void)p16;
    Packet_Value_Statistic p17;
    (void)p17;
    Packet_VillageMonsterFightResult p18;
    (void)p18;

    // ---- PassiveObjGenerator* 拷贝构造（隐式生成，force C1ERKS）----
    PassiveObjGeneratorObject o1;
    PassiveObjGeneratorObject o2(o1);
    (void)o2;
    PassiveObjGeneratorPattern n1;
    PassiveObjGeneratorPattern n2(n1);
    (void)n2;
    PassiveObjGeneratorSequence q1;
    PassiveObjGeneratorSequence q2(q1);
    (void)q2;

    // ---- PostalReward 拷贝构造（隐式生成，2 string + 2 int）----
    PostalReward r1;
    PostalReward r2(r1);
    (void)r2;

    // ---- PvPSkillTreeKey 3 参 ctor（Ehhh）----
    PvPSkillTreeKey k1((unsigned char)1, (unsigned char)2, (unsigned char)3);
    (void)k1;

    // ---- RandomApplicationKey 默认 ctor（两字节清零）----
    RandomApplicationKey a1;
    (void)a1;

    // ---- QuantityRatio / ResetValue / PartSelectonField 空析构（D1/D2）----
    {
        QuantityRatio t1;
        (void)t1;
        ResetValue t2;
        (void)t2;
        PartSelectonField t3;
        (void)t3;
    }

    // ---- SIG_* ctor ----
    SIG_ATTENDANCE_EVENT s1;
    (void)s1;
    SIG_BEST_CLEAR_TIME s2;
    (void)s2;
    SIG_BEST_CLEAR_TIME_DATA s3;
    (void)s3;
    SIG_BOOSTER_GAGE_DATA s4;
    (void)s4;
    SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER s5;
    (void)s5;
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER s6;
    (void)s6;
    SIG_CERASHOP_ERROR_LOG s7;
    (void)s7;
    SIG_COMBO_SKILL s8;
    (void)s8;

    // ---- OnlyOrigBatch14：头内联 operator=/clear 的 W 符号发射 ----
    // PassiveIObjSniperInfo::operator= W 0x08a3e76a
    PassiveIObjSniperInfo sniperA;
    PassiveIObjSniperInfo sniperB;
    sniperB = sniperA;
    // PassiveObjGeneratorObject/Pattern/Sequence::operator= W
    PassiveObjGeneratorObject o1b;
    PassiveObjGeneratorObject o2b;
    o2b = o1b;
    PassiveObjGeneratorPattern n1b;
    PassiveObjGeneratorPattern n2b;
    n2b = n1b;
    PassiveObjGeneratorSequence q1b;
    PassiveObjGeneratorSequence q2b;
    q2b = q1b;
    // LevelUpRewardItemTableKey::clear W 0x0891bad4
    LevelUpRewardItemTableKey key1;
    key1.clear();
    (void)key1;
}

}  // namespace
