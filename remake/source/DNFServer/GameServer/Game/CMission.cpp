// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMission/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 任务类族实现（G3-1 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMission*.md + ORIG 弱访问器反汇编。
#include <bitset>

#include "CMission.h"
#include "CMissionList_Charac.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "GlobalData.h"
#include "RefPvpGrade.h"   // RefPvpGrade 权威定义（GetPvpPoint/GetPvpNextRankPoint）

// cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};

// ============================================================================
// CMission
// ============================================================================

// b28d-50：ORIG 0x085e2ffc 发射序 = 声明序（标量 +4..+14 → +0x18/+0x20 两处
// 8 字节**整体拷贝** → +0x28/+0x2c → vector +0x30 → +0x3c）；旧「vector 只写
// meminit、标量留函数体」使 4.4 先发 vector C1（P239）。全部移入初始化列表且
// 按声明序书写，+0x18/+0x20 经 union 的 pair 视图整体拷贝（P246/P247）。
#include "CMission/CMission.inc"
}

#include "CMission/dtor_CMission.inc"
#include "CMission/_makeTotalMissionInfo.inc"
}

#include "CMission/_canUpdate.inc"
}

#include "CMission/_isAcceptCondition.inc"
}

#include "CMission/makeMissionInfo.inc"
}

#include "CMission/canUpdate.inc"
}

#include "CMission/isAcceptCondition.inc"
}

#include "CMission/Reward.inc"
}

#include "CMission/_Reward.inc"
}

bool CMission::operator<(const CMission& other)
{
    // ORIG W 0x085e30ae：!= 早退形（cmp;setne;test;je 续判，落 setl 返回），
    // 相等分支内联在 fall-through（P222 正向包裹；== 形产生镜像块序）。
    if (m_kind != other.GetMissionKind())
        return m_kind < other.GetMissionKind();
    if (m_missionIndex != other.GetMissionIndex())
        return m_missionIndex < other.GetMissionIndex();
    return false;
}

#include "CMission/_isAcceptCondition_Series.inc"
}

// ============================================================================
// CMission_rank（抽象，无 ctor；verifyMission 保持纯虚）
// ============================================================================

#include "CMission/dtor_CMission_rank.inc"

#include "CMission/isClearable.inc"
}

#include "CMission/isSeriesMission.inc"
}

#include "CMission/Update.inc"
}
// ORIG 0x85e6adc（W）：mov 0x40(%eax),%eax; ret —— 直返 m_clearCount。
#include "CMission/getClearCondition.inc"
}

// ORIG 0x85e372c：段位区间判定；两次独立 get_pvp_grade（P48 无 CSE），
// && 短路产生 jg/jge 直跳失败块 + 布尔返回归一。
#include "CMission/isAcceptCondition_1.inc"
}

// ============================================================================
// CMission_winning_count
// ============================================================================

#include "CMission/CMission_winning_count.inc"
}

#include "CMission/dtor_CMission_winning_count.inc"

#include "CMission/_makeTotalMissionInfo_1.inc"
}

#include "CMission/_canUpdate_1.inc"
}

#include "CMission/_isAcceptCondition_1.inc"
}

#include "CMission/isClearable_1.inc"
}

#include "CMission/verifyMission.inc"
}

#include "CMission/isSeriesMission_1.inc"
}

#include "CMission/Update_1.inc"
}

#include "CMission/getClearCondition_1.inc"
}

// ============================================================================
// CMission_winning_streak
// ============================================================================

#include "CMission/CMission_winning_streak.inc"
}

#include "CMission/dtor_CMission_winning_streak.inc"

#include "CMission/_makeTotalMissionInfo_2.inc"
}

#include "CMission/_canUpdate_2.inc"
}

#include "CMission/isClearable_2.inc"
}

#include "CMission/verifyMission_1.inc"
}

#include "CMission/isSeriesMission_2.inc"
}

#include "CMission/Update_2.inc"
}

#include "CMission/getClearCondition_2.inc"
}

// ============================================================================
// CMission_join
// ============================================================================

#include "CMission/CMission_join.inc"
}

#include "CMission/dtor_CMission_join.inc"

#include "CMission/_makeTotalMissionInfo_3.inc"
}

#include "CMission/_isAcceptCondition_2.inc"
}

#include "CMission/isClearable_3.inc"
}

#include "CMission/verifyMission_2.inc"
}

#include "CMission/isSeriesMission_3.inc"
}

#include "CMission/Update_3.inc"
}

#include "CMission/getClearCondition_3.inc"
}

// ============================================================================
// CMission_kill_count
// ============================================================================

#include "CMission/CMission_kill_count.inc"
}

#include "CMission/dtor_CMission_kill_count.inc"

#include "CMission/_makeTotalMissionInfo_4.inc"
}

#include "CMission/_isAcceptCondition_3.inc"
}

#include "CMission/isClearable_4.inc"
}

#include "CMission/verifyMission_3.inc"
}

#include "CMission/isSeriesMission_4.inc"
}

#include "CMission/Update_4.inc"
}

#include "CMission/getClearCondition_4.inc"
}

// ============================================================================
// CMission_hereafter_join
// ============================================================================

#include "CMission/CMission_hereafter_join.inc"
}

#include "CMission/dtor_CMission_hereafter_join.inc"

#include "CMission/isClearable_5.inc"
}

#include "CMission/verifyMission_4.inc"
}

#include "CMission/isSeriesMission_5.inc"
}

#include "CMission/Update_5.inc"
}

#include "CMission/getClearCondition_5.inc"
}

// ============================================================================
// CMission_hereafter_winning_count
// ============================================================================

#include "CMission/CMission_hereafter_winning_count.inc"
}

#include "CMission/dtor_CMission_hereafter_winning_count.inc"

#include "CMission/_canUpdate_3.inc"
}

#include "CMission/isClearable_6.inc"
}

#include "CMission/verifyMission_5.inc"
}

#include "CMission/isSeriesMission_6.inc"
}

#include "CMission/Update_6.inc"
}

#include "CMission/getClearCondition_6.inc"
}

// ============================================================================
// CMission_remain_hp
// ============================================================================

#include "CMission/CMission_remain_hp.inc"
}

#include "CMission/dtor_CMission_remain_hp.inc"

#include "CMission/isClearable_7.inc"
}

#include "CMission/verifyMission_6.inc"
}

#include "CMission/isSeriesMission_7.inc"
}

#include "CMission/Update_7.inc"
}

#include "CMission/getClearCondition_7.inc"
}

// ============================================================================
// CMission_revenge
// ============================================================================

#include "CMission/CMission_revenge.inc"
}

#include "CMission/dtor_CMission_revenge.inc"

#include "CMission/isClearable_8.inc"
}

#include "CMission/verifyMission_7.inc"
}

#include "CMission/isSeriesMission_8.inc"
}

#include "CMission/Update_8.inc"
}

#include "CMission/getClearCondition_8.inc"
}

// ============================================================================
// CMission_hereafter_kill_count
// ============================================================================

#include "CMission/CMission_hereafter_kill_count.inc"
}

#include "CMission/dtor_CMission_hereafter_kill_count.inc"

#include "CMission/isClearable_9.inc"
}

#include "CMission/verifyMission_8.inc"
}

#include "CMission/isSeriesMission_9.inc"
}

#include "CMission/Update_9.inc"
}

#include "CMission/getClearCondition_9.inc"
}

// ============================================================================
// CMission_kill
// ============================================================================

#include "CMission/CMission_kill.inc"
}

#include "CMission/dtor_CMission_kill.inc"

#include "CMission/_canUpdate_4.inc"
}

#include "CMission/isClearable_10.inc"
}

#include "CMission/verifyMission_9.inc"
}

#include "CMission/isSeriesMission_10.inc"
}

#include "CMission/Update_10.inc"
}

#include "CMission/getClearCondition_10.inc"
}

// ============================================================================
// CMission_within
// ============================================================================

#include "CMission/CMission_within.inc"
}

#include "CMission/dtor_CMission_within.inc"

#include "CMission/_isAcceptCondition_4.inc"
}

#include "CMission/isClearable_11.inc"
}

#include "CMission/verifyMission_10.inc"
}

#include "CMission/isSeriesMission_11.inc"
}

#include "CMission/Update_11.inc"
}

#include "CMission/getClearCondition_11.inc"
}

// ============================================================================
// CMission_remains_hp
// ============================================================================

#include "CMission/CMission_remains_hp.inc"
}

#include "CMission/dtor_CMission_remains_hp.inc"

#include "CMission/_isAcceptCondition_5.inc"
}

#include "CMission/isClearable_12.inc"
}

#include "CMission/verifyMission_11.inc"
}

#include "CMission/isSeriesMission_12.inc"
}

#include "CMission/Update_12.inc"
}

#include "CMission/getClearCondition_12.inc"
}

// ============================================================================
// CMission_move_channel
// ============================================================================

#include "CMission/CMission_move_channel.inc"
}

#include "CMission/dtor_CMission_move_channel.inc"

#include "CMission/_isAcceptCondition_6.inc"
}

#include "CMission/isClearable_13.inc"
}

#include "CMission/verifyMission_12.inc"
}

#include "CMission/isSeriesMission_13.inc"
}

#include "CMission/Update_13.inc"
}

#include "CMission/getClearCondition_13.inc"
}

// ============================================================================
// CMission_move_channel_total
// ============================================================================

#include "CMission/CMission_move_channel_total.inc"
}

#include "CMission/dtor_CMission_move_channel_total.inc"

#include "CMission/_isAcceptCondition_7.inc"
}

#include "CMission/isClearable_14.inc"
}

#include "CMission/verifyMission_13.inc"
}

#include "CMission/isSeriesMission_14.inc"
}

#include "CMission/Update_14.inc"
}

#include "CMission/getClearCondition_14.inc"
}

// ============================================================================
// CMission_combo_clear
// ============================================================================

#include "CMission/CMission_combo_clear.inc"
}

#include "CMission/dtor_CMission_combo_clear.inc"

#include "CMission/_makeTotalMissionInfo_5.inc"
}

#include "CMission/_isAcceptCondition_8.inc"
}

#include "CMission/isClearable_15.inc"
}

#include "CMission/verifyMission_14.inc"
}

#include "CMission/isSeriesMission_15.inc"
}

#include "CMission/Update_15.inc"
}

#include "CMission/getClearCondition_15.inc"
}

// [round3-L29 2026-09-10 复核] 我方 127 / ORIG 125：difflib 显示我方多两段
//   「mov $0x1,%ebx; jmp」/「mov $0x1,%ebx; lea; call」= 早返回 true 的物化。
// [2026-09-10 round5 定稿 **已修**] 按 P292 正极性包裹改写：ORIG 0x085e41af
//   `test %al,%al; je 085e42e1`（je 落**函数尾部共享块** `mov $0x1,%eax`，cond 真
//   走 body 直落）⇒ 源形必须是 `if (it != m_comboMap.end()) { body } return 1;`
//   （早返回 `if (end()==it) return 1;` 会就地物化 mov $1,%ebx + jmp）。
//   改后：origN=ourN=125、帧 80=80、判据⑪ PURE SHIFT c=0；.o 口径 DIFF→**NEAR**
//   （残余仅 2 处字符串地址重定位伪差：ORIG 0x8cc2c5c/0x8cc2c70 vs 我方 .o 的
//   addend 0x0/0x14 = N124/N132）⇒ 链接口径 AE。
#include "CMission/_Reward_1.inc"
}

// ORIG W 0x085e692a：sret 返回 std::pair<int,int>（8 字节）。ORIG 为逐字段内联拷贝
// （`mov 0x1c(%eax),%edx; mov 0x18(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx);
// mov %ecx,%eax; ret $0x4`）——pair<int,int> 拷贝构造 trivial ⇒ 无 call，与 C1
// 0x085e304b 同族。成员已按 ORIG 建模为 std::pair<int,int> m_rankRange（b28d-70）。
// [b28d-70] 判词：IDENTICAL 11=11。
#include "CMission/GetRankRange.inc"
}
