// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkSSO_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// WongWork::CSimpleSSO 方法集（G3 批次 A，ORIG df_game_r）。
// 报告：docs/class_func_reports/WongWork__CSimpleSSO/
//   expireLoginTime 0x08602b24 / reset 0x086028b4 / checkValidPassword 0x086028c6 /
//   updateSSOExpireTime(uint,st) 0x08602906 / updateSSOExpireTime(uint,uint) 0x08602a4a /
//   _checkValiedPassword 0x08602bd0 / _getGameStartTime 0x08602da6 /
//   getGameStartTime 0x086031f6 / updateHumanCertify 0x08603226 /
//   checkValidPassword_Jpn 0x081b25ec / stOutputData_t::ctor(W) 0x0818fb28。
// 类权威声明：CSimpleSSO.h（原 GlobalData.cpp 局部类收敛）；ctor/dtor/init 属 GlobalData.cpp。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射。
// [T1-GI · P327] 键名 `_GLOBAL__I__ZN8WongWork10CSimpleSSO22checkValidPassword_Jpn
// EjPKcS2_RNS0_14stOutputData_tE`（ORIG 0x81b26e8）：checkValidPassword_Jpn 定义前移为
// 本 TU 首个「强符号 · 外部链接 · 本 TU 定义」实体（改前首个 = stOutputData_t ctor）。
#include <iostream>

#include "CSimpleSSO.h"
#include "MySQL.h"
#include "CSystemTime.h"
#include "CGM_Manager.h"     // CGM_Manager::IsGm
#include "CRijndael.h"
#include "DNFFunctionLib.h"  // DNFFLib::Binary2Hex
#include "LogManager.h"      // LogManager::logFormat
#include "GameTypes.h"       // cMyTrace

// 轻量 extern 声明（避免拉入 CUser.h/GlobalData.h 重型共享头）：
char* NumberToString(unsigned int value, int radix);          // ORIG 0x0810904b
namespace GlobalData
{
extern CSystemTime s_systemTime_;      // 0x0941f714
extern CGM_Manager* s_GM_Manager;      // 0x0941f75c
}


#include <string.h>
#include <stdio.h>

// ---- checkValidPassword_Jpn(...)  ORIG 0x081b25ec（键名位 · P327：定义前移）
#include "WongWorkSSO_A/checkValidPassword_Jpn.inc"
}

// ---- stOutputData_t::stOutputData_t()  ORIG W 0x0818fb28：逐字段置零，
//      m_loginIp(+0x18)/pad 区仅清首字节，与 ORIG 指令序一致。
#include "WongWorkSSO_A/stOutputData_t.inc"
}

// ---- reset()  ORIG 0x086028b4：*(this+0xd0860)=0 → m_mysql=0
#include "WongWorkSSO_A/reset.inc"
}

// ---- expireLoginTime(mId)  ORIG 0x08602b24
#include "WongWorkSSO_A/expireLoginTime.inc"
}

// ---- _getGameStartTime(mId, login&, expire&, out&)  ORIG 0x08602da6
#include "WongWorkSSO_A/_getGameStartTime.inc"
}

// ---- getGameStartTime(...)  ORIG 0x086031f6：薄转发 _getGameStartTime
// [b23S3C] bool 透传：ORIG 体末 `call _getGameStartTime; leave; ret`（%eax 直带
// al 结果，无槽），调用点 0x0818f7a8 xor$1/test%al 实证返回承载成败。
#include "WongWorkSSO_A/getGameStartTime.inc"
}

// ---- checkValidPassword(...)  ORIG 0x086028c6：两相邻 unsigned 局部作 long& 传入；
// ORIG 返回 bool（= _getGameStartTime 结果，GetAccountInfo 失败时记日志返回 false）。
#include "WongWorkSSO_A/checkValidPassword.inc"
}

// ---- _checkValiedPassword(pwd, clientHash, cipher)  ORIG 0x08602bd0
#include "WongWorkSSO_A/_checkValiedPassword.inc"
}

// ---- updateHumanCertify(mId, accountFail)  ORIG 0x08603226
#include "WongWorkSSO_A/updateHumanCertify.inc"
}

// ---- updateSSOExpireTime(mId, data)  ORIG 0x08602906
#include "WongWorkSSO_A/updateSSOExpireTime.inc"
}

// ---- updateSSOExpireTime(mId, loginTime)  ORIG 0x08602a4a
#include "WongWorkSSO_A/updateSSOExpireTime_1.inc"
}
