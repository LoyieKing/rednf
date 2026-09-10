// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Sanicova/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ Sanicova 命名空间类实现（独立 TU，从 GameStubs.cpp 拆出）。
// LinkMisc5 收口：CPad 全字段访问器真实化（ORIG 反汇编逐条核对），并承载
// WongWork::CSecurityCard 补充方法与 CUser::sendBackSecurityAuthRequest。
#include "CUser.h"

#include "Cipher.h"
#include "GameRand.h"    // get_rand_int
#include "GlobalData.h"  // s_systemTime_ / s_secu_cipher_keystring_manager_
#include "PacketGuard.h"

namespace Sanicova
{

// ORIG T 0x0859957c _ZN8Sanicova4CPad4initEv：
// activate/certified 清零 → memset(this,0,0x24) → failCnt/requestState/cancelCnt
// 清零 → +0x2c/+0x26/+0x2d 字节清零 → lastGenTime=getCurSec() → advide/changedPwd
// 清零 → GenerateRandomKeyIdx → GenerateRandomShakeData。
#include "Sanicova/init.inc"
}

// ORIG T 0x08599616 _ZN8Sanicova4CPad5resetEv：已认证时读取 +0x28 认证时间戳，
// 有效期 0xb4（180s）：checkTime+0xb4 < 当前秒则 setCertified(false)；前后各打
// cMyTrace 日志（[Before]/[After]Goblin Certified，line 65/74）。
#include "Sanicova/reset.inc"
}

// ORIG T 0x0859979c _ZN8Sanicova4CPad19isTimeAttackSeccessEv：取全局当前秒
// （结果存局部未参与判定），恒返回 true。
#include "Sanicova/isTimeAttackSeccess.inc"
}

#include "Sanicova/getCancelCnt.inc"

#include "Sanicova/incCancelCnt.inc"

#include "Sanicova/setCancelCnt.inc"

#include "Sanicova/getRequestState.inc"

#include "Sanicova/setRequestState.inc"
}

#include "Sanicova/isActivate.inc"

#include "Sanicova/setActivate.inc"

#include "Sanicova/isCertified.inc"

#include "Sanicova/setCertified.inc"

#include "Sanicova/getFailCnt.inc"

#include "Sanicova/setFailCnt.inc"

#include "Sanicova/getVersionInfo.inc"

#include "Sanicova/setVersionInfo.inc"
#include "Sanicova/getAdvideChangePwd.inc"
}

// ORIG W 0x084ebdb2：写 +0x3c 字节。
// P210：本 setter 与 setChangedPwd 定义位于 SendChagePwdRewardMail 之后，
// 使 4.4 对该 callee 采用保守帧（ORIG sub $0x18）。
// ORIG T 0x0859981e：置密改与建议标记为 0（发奖邮件已由上层处理）。
// 定义前置到 setter 之前（P210 callee 定义顺序定律）。
#include "Sanicova/SendChagePwdRewardMail.inc"
}

#include "Sanicova/setAdvideChangePwd.inc"
}

// ORIG W 0x084ebdca：movzbl 0x3d(%eax) 直返 +0x3d 字节。
#include "Sanicova/getChangedPwd.inc"
}

// ORIG T 0x085998a6：当前秒距上次生成 >20s 判有效并刷新时间戳。
#include "Sanicova/isValidKeyGenTime.inc"
}

// ORIG T 0x08599918：+0x2e = get_rand_int(5000)。
#include "Sanicova/GenerateRandomKeyIdx.inc"
}

// ORIG T 0x08599946：+0x34[i] = get_rand_int(200)+20，i∈[0,4)。
#include "Sanicova/GenerateRandomShakeData.inc"
}

// ORIG T 0x085998ea：key 生成时间有效时刷新 keyIdx 与扰动数据。
#include "Sanicova/GenerateRandomData.inc"
}

// ORIG T 0x0859984c：min(+0x2e, 0x1387)。
#include "Sanicova/GetSelectedCipherIdx.inc"
}

// ORIG T 0x08599872：按 +0x2e 索引读全局密钥管理器表（0x940be80 基址，
// 步长 16 字节；idx==0 取 +4 槽、否则取 +8 槽 → manager[key*4+1/2]）。
#include "Sanicova/GetKeyIdx.inc"
}

// ORIG T 0x08599936：读 +0x34[idx] 字节。
#include "Sanicova/GetShakeData.inc"
}

#include "Sanicova/setChangedPwd.inc"
}


// ORIG T 0x085997b8：建议改密时复位建议并触发发奖（无角色则仅置密改标记）。
#include "Sanicova/SendChagePwdReward.inc"
}

}  // namespace Sanicova

// ===========================================================================
// WongWork::CSecurityCard 补充方法（LinkMisc5）
// ===========================================================================

// ORIG W 0x0822ef58：读 +0x44。
#include "Sanicova/getRequestState_1.inc"
}

// ORIG T 0x085fddb2：问题四元组合法性检查后导出（越界/全零则重置为 0,0,1,1）。
#include "Sanicova/generateQuestion.inc"
}

// ===========================================================================
// CUser::sendBackSecurityAuthRequest（报告 CUser/sendBackSecurityAuthRequest.md，
// ORIG T 0x08680706 sz 0x38f）：生成安全卡问题 → 随机加密算法号 → Pad 随机
// 数据 → Cipher 加密 → 0x98 包下发 → 置请求状态。
// ===========================================================================
#include "Sanicova/sendBackSecurityAuthRequest.inc"
}
