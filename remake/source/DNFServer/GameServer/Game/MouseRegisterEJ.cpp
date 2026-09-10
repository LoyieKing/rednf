// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MouseRegisterEJ/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// MouseRegisterEJ.cpp — 台湾 MouseRegister 域 + expert_job 统计（LinkClosureB）
//
// 报告来源（逐份转写，汇编+反编译 C）：
//   docs/class_func_reports/DB_MouseRegister/{convertPassPadInfo,
//       _getDBHandleForDevideAccountDB,_updateFailCnt,_updateHistory}.md
//     DB_MouseRegister::convertPassPadInfo        T 0x08422e6a（static）
//       c++filt 权威形参：(unsigned int, char*, int&, char&, int&, bool&,
//       char*, bool&, char*, long&, bool&)；mangled
//       _ZN16DB_MouseRegister18convertPassPadInfoEjPcRiRcS1_RbS0_S3_S0_RlS3_
//       （返回值以 int 错误码承载：失败计数字路径 eax=0x48/0x49 被上层 dispatch
//       直接消费 ==0x48→errcode5、==0xa6→9；mangled 不含返回类型）
//     DB_MouseRegister::_getDBHandleForDevideAccountDB T 0x08422938
//     DB_MouseRegister::_updateFailCnt            T 0x084231b6
//     DB_MouseRegister::_updateHistory            T 0x08423914
//     encryptAES_(CRijndael*, char*, char*)       T 0x0846c398
//   docs/class_func_reports/expert_job__CEnchanter/_TryEnchant.md
//     expert_job::CEnchanter::_TryEnchant         T 0x0849dcc4
//   docs/class_func_reports/{CExpertJobStatistic,_lt_global_gt_}/
//     CExpertJobStatistic C1 W 0x0861074a / D1 W 0x086108ae
//     ExpertJobKey C1 W 0x08610720 / operator< W 0x086106da
//     increaseItemCount                            T 0x0860f9d4
//     GetInstanceExpertJobStatistic                T 0x0860f940（__cxa_guard 单例，
//                                                  guard@0x09473320 instance@0x09473350）
//
// 说明：IMethod::GetBlockSize/IMethod::Pad 定义已在 DBMW/Method.cpp（mangled 一致）；
// getItemIndex 已在 HeroMissionLink4.cpp；本 TU 均只复用不重复定义。
// ============================================================================

#include "MySQL.h"
#include "DBMgr.h"
#include "CRijndael.h"
#include "GlobalData.h"
#include "CEnvironment.h"      // IMethod / ENUM_DB_HANDLE_IDX
#include "LogManager.h"
#include "../../ServerCommon/DNFFunctionLib.h"   // DNFFLib::Binary2Hex
#include "CInventory.h"        // NumberToString 声明
#include "CSystemTime.h"
#include "STItemScript.h"      // ENUM_EXPERT_JOB_TYPE
#include "CExpertJob.h"
#include "SigTypes.h"          // SIG_MOUSE_REGISTER / SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD
#include "CStreamGuard.h"      // RAII 消息封装（_sendResult/dispatch/_modifyPassword）
#include "MsgQueueMgr.h"       // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "StreamPool.h"

#include <cstring>
#include <map>
// ---------------------------------------------------------------------------
// DB_CancelRestingUserRestrict / DB_CheckSecuReward —— ORIG T 段 makeRequest
// （_ZN28DB_CancelRestingUserRestrict11makeRequestEji @0x0843741e、
//   _ZN18DB_CheckSecuReward11makeRequestEiji @0x08444408）。完整类与 m_dispatchers
//   登记在 W8LT_DBTask.cpp；本 TU 仅按 ORIG 签名局部声明 static makeRequest 供调用，
//   定义归 W8LT_DBTask.cpp（勿重复实现）。
// ---------------------------------------------------------------------------
class DB_CancelRestingUserRestrict
{
public:
    static void makeRequest(unsigned int uid, int param2);
};

class DB_CheckSecuReward
{
public:
    static void makeRequest(int param1, unsigned int uid, int param3);
};

// 写入视图（P82 族）：+0x00 uid / +0x04 param3，使 4.4 折叠为单条 disp store。
struct SIG_CSR_WRITEVIEW
{
    unsigned int m_uid;
    int m_param3;
};

#include "MouseRegisterEJ/makeRequest.inc"
}

#include "MouseRegisterEJ/makeRequest_1.inc"
}

// ---------------------------------------------------------------------------
// SIG_MR_WRITEVIEW —— SIG_MOUSE_REGISTER 的 +0x16..+0x22 写入视图（P1 组件视图，
// 使 movl/movb 折叠为单指令 disp；SigTypes_D.h 为共享头勿动）。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct SIG_MR_WRITEVIEW
{
    unsigned int m_accId;    // +0x00
    unsigned int m_pkgType;  // +0x04
    char m_pad08[0xe];       // +0x08..+0x15
    int m_field16;           // +0x16（movl）
    char m_field1a;          // +0x1a（movb）
    int m_field1b;           // +0x1b（movl）
    int m_field1f;           // +0x1f（movl）
};
#pragma pack(pop)
// ---------------------------------------------------------------------------
// encryptAES_ —— ORIG T 0x0846c398（encryptAES_.md 反编译转写）。
// 结果 0x6fffffff 成功转 hex（长度=out*2+1）；否则 logFormat("Encrypt Error") 且 out=NULL。
// 虚槽直调按 P5（vt+0xc = IMethod::Encrypt），__FUNCTION__ 对齐 _ZZ...E12__FUNCTION__。
// ---------------------------------------------------------------------------
char* encryptAES_(CRijndael* method, char* src, char* dst)
{
    unsigned char encBuf[132];
    size_t len;
    size_t padded;
    int block;
    int ret;

    len = strlen(src);
    padded = 0;
    block = method->GetBlockSize(static_cast<unsigned int*>(0));
    if (block >= 0)
    {
        if (static_cast<int>(len) % block != 0)
        {
            padded = static_cast<size_t>(
                (static_cast<int>(len) / block + 1) * block);
        }
        else
        {
            padded = len;
        }
    }
    else
    {
        padded = len;
    }
    if (block >= 0)
    {
        method->Pad(src, static_cast<int>(len),
                    static_cast<unsigned int*>(0));
    }
    typedef int (*Fn)(const void*, const char*, char*, size_t);
    ret = (*(Fn*)(*(void* const*)method + 0x0c))(method, src, (char*)encBuf, padded);
    if (ret != static_cast<int>(0x6fffffff))
    {
        LogManager::logFormat(1, "DesignPattern.cpp", __FUNCTION__, 0x9e,
                              "[%s][%d][%s]", __FUNCTION__, 0x9e,
                              "Encrypt Error");
        return static_cast<char*>(0);
    }
    len = padded * 2 + 1;
    DNFFLib::Binary2Hex(encBuf, static_cast<int>(padded), dst);
    *(dst + len) = '\0';
    return dst;
}

// ============================================================================
// DB_MouseRegister（台湾鼠标密码注册域；ORIG 完整类带虚表，仅本域方法被
// 全工程引用——无跨 TU 实例构造调用点，故此处不需要产出 vtable/typeinfo；
// 非 static 成员为 i386 cdecl 形态（this 入栈），与 ORIG objdump 一致）
// ============================================================================
class DB_MouseRegister
{
public:
    // ORIG T 0x08422e6a：convertPassPadInfo 主体（见头注释的错误码口径）。
    static int convertPassPadInfo(unsigned int mid, char* mousepass,
                                  int& failCnt, char& cancelCnt,
                                  int& cancelCnt2, bool& validityExpired,
                                  char* ip, bool& flag2, char* buf3,
                                  long& validityTime, bool& flag3);
    // ORIG T 0x08422c8a：查询/校验鼠标密码；错误码 1/0x49/0xa6。
    static int checkPassword(unsigned int mid, const char* mousepass,
                             bool param3, int& failCnt, int& cancelCnt,
                             bool& validityExpired, char* ip, char* buf3);
    // ORIG T 0x08423abe：回写 SIG_MOUSE_REGISTER 并投递 DISPATCH_Q。
    void _sendResult(unsigned int mid, int param2, int param3, int param4,
                     SIG_MOUSE_REGISTER* param5);
    // ORIG T 0x08422964：读流、按 pkgType 分派注册/改密/解绑并回执。
    int dispatch(int param1, int param2, Stream* stream);
    // ORIG T 0x08423528：改密（version_info 保持 '2'，写 security_grade）。
    int _modifyPassword(unsigned int mid, const char* pwd, char* ip,
                        int param4, bool param5);
    // ORIG T 0x084232e0：注册新密码（不存在 insert，存在则 update）。
    int _registNewPassword(unsigned int mid, const char* pwd, char* ip,
                           int param4);
    // ORIG T 0x08423794：解绑（删除记录 + 清 security_grade 时间）。
    int _unregistPassword(unsigned int mid, char* ip);

private:
    // ORIG T 0x08422938：DeviceAccountDB 句柄取回。
    static MySQL* _getDBHandleForDevideAccountDB();
    // ORIG T 0x084231b6：mousepass.fail_cnt 与 security_grade.goblin_fail_cnt 同步。
    // 返回 bool（ORIG 尾 0x084232dd `test %al,%al; leave; ret` 无 movzbl
    // 规范化 ⇒ bool 直返；调用点仅语句式消费，mangled 不含返回类型）。
    static bool _updateFailCnt(unsigned int mid, int failCnt, char* ip);
    // ORIG T 0x08423914：写 member_mousepass_history_%Y%m 月份表。
    static bool _updateHistory(unsigned int mid, int modifyType, char* ip);
};

#include "MouseRegisterEJ/_getDBHandleForDevideAccountDB.inc"
}

// [b28d-47 / b28c-R26 复验] verdict=DIFF(routed) [N93附 + P168a③] ORIG 0x084231b6
//   orig/ours=74/75；p167 调用点差集 CLEAR（ORIG calls=12 / OUR calls=12，无 MISSING/EXTRA）。
//   残差两处均为跨版本 codegen、无源形解：
//   ① ORIG 尾 0x084232db `test %al,%al`（紧随 `xor $0x1,%eax`）是 4.1 对
//      `return !exec()` 的 bool 归一落空指令（dead test，无分支消费者）；
//      P168a③ 总闸实测 4.4 恒以 `movzbl %al,%eax` 加宽顶掉 dead test。
//   ② ORIG 的 `mov $0,%eax`(0x0842328b) 同时服务条件汇聚 r=false 与 `return false`
//      （4.1 尾常量共享，函数只有一处 mov $0）；4.4 恒在函数尾另发 `mov $0,%eax`
//      并使 insert 块尾多发 `jmp` 中继（N93附 尾块发射序）。
//   已试形（本轮复验）：① if/else 具名 else{return false;}（四数不变，仍 DIFF）；
//   ② 早返回正向形 / `if(exec) return false;`（判词记录 79 insn 反劣，mov$0/mov$1 双岛）。
#include "MouseRegisterEJ/_updateFailCnt.inc"
}

#include "MouseRegisterEJ/_updateHistory.inc"
}

#include "MouseRegisterEJ/convertPassPadInfo.inc"
}

// ============================================================================
// DB_MouseRegister 剩余 6 符号（ORIG T 段全转写）：
//   checkPassword 0x08422c8a（static，this 未入栈）、
//   _sendResult 0x08423abe / dispatch 0x08422964 / _modifyPassword 0x08423528 /
//   _registNewPassword 0x084232e0 / _unregistPassword 0x08423794（非 static，
//   i386 cdecl：this 入栈 0x8，参数自 0xc 起——与 ORIG objdump 逐槽核对）。
// ============================================================================

#include "MouseRegisterEJ/checkPassword.inc"
}

#include "MouseRegisterEJ/_registNewPassword.inc"
}

#include "MouseRegisterEJ/_modifyPassword.inc"
}

#include "MouseRegisterEJ/_unregistPassword.inc"
}

#include "MouseRegisterEJ/_sendResult.inc"
}

#include "MouseRegisterEJ/dispatch.inc"
}

// ============================ expert_job 域 ==================================

namespace expert_job {

// ORIG T 0x0849dcc4（_TryEnchant.md / objdump 全文核对）。分派三种试附魔路径：
//   type==0 自家材料：_TryEnchantMine 后触发基类虚槽1（vptr+0x04）(user,0)；
//   type==1 对家材料：_TryEnchantOther，失败时复位材料②(this+0x18)并清登记配方(+0x40)；
//   type==2 仅虚槽(user,2)；其它类型无副作用。
// 虚槽经 vptr 偏移 +0x04 显式调用：与 ORIG 指令逐条对应；CExpertJob 虚表槽序
// 审计收敛后可替换回语义虚调用。
// [b28d-47 / b28c-R26 复验] verdict=NEAR(routed) [N7] ORIG 0x0849dcc4 orig/ours=64/64
//   本轮改动（DIFF→NEAR，两处）：
//   ① if/else-if 链 → switch(type)：ORIG 0x0849dcd1 先 `mov 0x14(%ebp),%eax` 再
//      寄存器比较链（cmp $1/je、cmp $2/je、test/jne），三叶块物理序 = case 书写序
//      0→1→2（ORIG 0x0849dcea case0 直落 / +0x65 case1 / +0xa6 case2）⇒ P251/P286/P324；
//   ② case1 内 `if (ret != 0)` → `while (ret != 0) { …; break; }`：复现 ORIG
//      0x0849dd45 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`（P225）。
//   残差 = N7（4.4.7 间接 call 目标恒 %eax；ORIG 0x0849dd0e `mov (%eax),%edx` +
//   `call *%edx`，我方 `mov (%eax),%eax` + `call *%eax`），属寄存器角色跨版本差。
#include "MouseRegisterEJ/_TryEnchant.inc"
}

}  // namespace expert_job

// ------------------------------ 统计单例 ------------------------------------

// ORIG C1 W 0x0861074a：map 默认构造后紧接 clear()（双调用形态保留）。
#include "MouseRegisterEJ/CExpertJobStatistic.inc"
}

// ORIG D1 W 0x086108ae：map 逆序析构（编译器生成等价）。
#include "MouseRegisterEJ/dtor_CExpertJobStatistic.inc"
#include "MouseRegisterEJ/ExpertJobKey.inc"
}

// ORIG operator< W 0x086106da：int 有符号前置序 + unsigned long 无符号次序。
bool CExpertJobStatistic::ExpertJobKey::operator<(const ExpertJobKey& other) const
{
    if (m_field00 < other.m_field00)
        return true;
    if (m_field00 == other.m_field00 &&
        m_itemIdx04 < other.m_itemIdx04)
    {
        return true;
    }
    return false;
}

// ORIG T 0x0860f9d4（increaseItemCount.md）：find 未命中则以 count 为初值插入，
// 否则 value += count。
#include "MouseRegisterEJ/increaseItemCount.inc"
}

// ORIG T 0x0860f940：__cxa_guard 单例（instance/guard BSS 0x09473350/0x09473320）。
CExpertJobStatistic* GetInstanceExpertJobStatistic()
{
    static CExpertJobStatistic instance;
    return &instance;
}

// ORIG T 0x0860faca（UpdateExpertJob.md）：map 空直接返回；否则 Acquire
// ("Statistics.cpp", 0x65a) 后写头 0x182/-1/map.size()，遍历逐条写
// {key.m_field00, key.m_itemIdx04, value}，put(DB_Q) 后 clear()。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x0860faca orig/ours=137/137
//   残差：槽/寄存器分配跨版本差（指令数全等，语义对齐）。
#include "MouseRegisterEJ/UpdateExpertJob.inc"
}
