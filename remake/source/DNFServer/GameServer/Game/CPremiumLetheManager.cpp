// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPremiumLetheManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPremiumLetheManager（G2-4 批次，忘川水/付费技能重置管理）
// 参照 docs/class_func_reports/CPremiumLetheManager.md 与 ORIG 反汇编逐函数实现。
// 原源文件 PremiumLetheManager.cpp；数据全局态经 DB 队列（_update）下账。
#include <string.h>

#include <vector>

#include "CDataManager.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "SkillSlot.h"
#include "StreamPool.h"
#include "GameWorld.h"
#include "PacketGuard.h"

// ---- 全局单例（CUserCharacInfo.h 已声明 namespace GlobalData；不引入
//      GlobalData.h 以避免 class/namespace 形态冲突） ----
// [R18gd 2026-09-13] GlobalData 已按 ORIG 形态 class 化（namespace→class 静态成员）；
// 原此处 `namespace GlobalData { extern ... }` 最小声明重开点合并到权威头（唯一声明点）
// ——符号名/mangled 与 `GlobalData::x` 访问语法零变化。
#include "GlobalData.h"

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；构造/析构由
// GameStubs 提供桩符号，本 TU 只声明不定义）
// ============================================================================


// ---- 忘川水状态（ORIG 数据段 .data 0x93abc60 / .bss 0x9472dbc）----
class CPremiumLetheManager
{
public:
    CPremiumLetheManager();
    ~CPremiumLetheManager();

    bool ResetSkillReq(CUser* user);
    bool ConfirmSkillReq(CUser* user);
    bool BackToPre(CUser* user, _Mastered_skill* skills,
                   ENUM_SKILL_TREE_KIND kind);
    bool UpdateBackupSkillFlag(CUser* user, ENUM_SKILL_TREE_KIND kind);
    bool InitLetheSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _resetSkill(CUser* user);
    void InitSkill(CUser* user, bool flag, ENUM_SKILL_TREE_KIND kind);
    bool BackupSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _update(CUser* user, unsigned char state, ENUM_SKILL_TREE_KIND kind);
    void _removeInitSkill(CUser* user, _Mastered_skill* skills);

    static unsigned char USE_LETHE_STATE;       // +0x093abc60 = 1
    static unsigned char SKILL_CONFIRM_STATE;   // +0x093abc61 = 2
    static unsigned char BACK_TO_PRE_STATE;     // +0x093abc62 = 3
    static unsigned char INIT_LETHE_SKILL;      // +0x093abc63 = 4
    static unsigned char NOT_USE_LETHE_STATE;   // +0x09472dbc = 0
};

#include "CPremiumLetheManager/CPremiumLetheManager.inc"


unsigned char CPremiumLetheManager::USE_LETHE_STATE = 1;
unsigned char CPremiumLetheManager::SKILL_CONFIRM_STATE = 2;
unsigned char CPremiumLetheManager::BACK_TO_PRE_STATE = 3;
unsigned char CPremiumLetheManager::INIT_LETHE_SKILL = 4;
unsigned char CPremiumLetheManager::NOT_USE_LETHE_STATE = 0;

// SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD 权威定义已迁移至 SigTypes.h
// （FixMissingSigTypes；DB 队列负载，pack(1)，sizeof=0x1a7）。

// ---- CUser 方法声明见 CUser.h（本 TU 直接真实调用） ----

#include "CPremiumLetheManager/dtor_CPremiumLetheManager.inc"


#include "CPremiumLetheManager/ResetSkillReq.inc"


#include "CPremiumLetheManager/ConfirmSkillReq.inc"


#include "CPremiumLetheManager/BackToPre.inc"


#include "CPremiumLetheManager/UpdateBackupSkillFlag.inc"


#include "CPremiumLetheManager/InitLetheSkill.inc"


#include "CPremiumLetheManager/_resetSkill.inc"


#include "CPremiumLetheManager/InitSkill.inc"


#include "CPremiumLetheManager/BackupSkill.inc"


#include "CPremiumLetheManager/_update.inc"


#include "CPremiumLetheManager/_removeInitSkill.inc"

