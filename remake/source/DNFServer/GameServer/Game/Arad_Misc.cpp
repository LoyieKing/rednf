// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Arad_Dispatcher.h"
#include "Arad_ItemBonus.h"

#include "CRijndael.h"      // IMethod（GetBlockSize/Pad/Encrypt 虚槽2）
#include "CStreamGuard.h"
#include "SigTypes.h"       // SIG_MAILBOX_SEND_NEW_MAIL
#include "GlobalData.h"     // GlobalData::s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "GameTypes.h"      // cMyTrace
#include "Inven_Item.h"

#include "../../ServerCommon/DNFFunctionLib.h"  // DNFFLib::Binary2Hex

extern "C" {
#include <arpa/inet.h>
}
#include <ctime>
#include <cstring>
#include <string>

namespace ARAD {

namespace ARAD_CRYPT {

char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    // ORIG 0x08184abc（ARAD__ARAD_CRYPT/encryptAES.md）：与 ARAD::encryptAES
    // 指令体相同，仅 logFormat 文案不同（localjapan/Arad_Crypt.cpp、"encryptAES"、
    // 0x1f），按报告各自独立实现。
    // [AradMisc-E5~E9 2026-09-11 · DIFF(91)→**NEAR** 89/89 gap0] 六旋钮落地（ORIG 证据）：
    //   D1 0x08184ad3 `movl $0x0,-0x14` ⇒ `int paddedLen = 0;`（原缺该条）；
    //   D2 0x08184b0f `mov %eax,%edx; sar $0x1f,%edx; idivl` ⇒ srcLen/paddedLen 用 **int**
    //      （size_t 时 4.4 发 `mov $0x0,%edx; divl` + 额外 -0xac 中转，多 2 条；离差整 27→0）；
    //   D3 0x08184bdc 成功后 hexLen 写 -0x18（= srcLen 槽）⇒ 源码**复用同一变量** `srcLen`；
    //   D4 0x08184af4 `js +0x6a` ⇒ `if (blockSize >= 0) {...} else {...}` 嵌套形（else-if 链产 `jns`+then 前置）；
    //   D5 0x08184b05 `je +0x62` ⇒ 内层 `if (mod != 0) { 计算 } else { paddedLen = srcLen; }`（then 后置）；
    //   D6 0x08184b87 `je +0x118` ⇒ `if (result != 0x6fffffff) { log; return NULL; }`（`==` 式产 `jne`+then 前置）。
    // 残余 NEAR：0x08184b5b 段 Encrypt 实参装载的寄存器选择（ORIG fn→%edx、实参→%eax；
    //   我方 fn→%eax、实参→%edx/%ecx），助记符逐行一致、仅寄存器不同。
    char enc[132];
    int srcLen = strlen(inData);
    int paddedLen = 0;
    int blockSize = cipher->IMethod::GetBlockSize(NULL);
    if (blockSize >= 0) {
        if (srcLen % blockSize != 0) {
            paddedLen = (srcLen / blockSize + 1) * blockSize;
        } else {
            paddedLen = srcLen;
        }
    } else {
        paddedLen = srcLen;
    }
    if (blockSize >= 0) {
        cipher->IMethod::Pad(inData, srcLen, NULL);
    }
    // [P6AuditOldN-S1 2026-09-11 负结论] 试显式基类限定调用 `cipher->IMethod::Encrypt(...)`
    //   （目标 ORIG 0x08184b5b 段 vptr→%edx / 实参走 %eax）⇒ 实测 our 89→**85**
    //   （gap 0→**+4**，且 ORIG 该段实为 `mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`
    //   vtable+0xc 槽形）⇒ 反劣，已回滚，维持 NEAR（残余 = 4.4 实参装载寄存器选择）。
    int result = cipher->Encrypt(inData, enc, paddedLen);
    if (result != 0x6fffffff) {
        // [Fn28Gap 2026-09-11] ORIG 0x08184b99: movl $0x8b8f790,0x14(%esp) =
        // _ZZN4ARAD10ARAD_CRYPT10encryptAESEP9CRijndaelPcS3_E12__FUNCTION__
        // （r,11B="encryptAES\0"）⇒ funcName 位为宏 __FUNCTION__。
        LogManager::logFormat(
            1, "localjapan/Arad_Crypt.cpp", __FUNCTION__, 0x1f,
            "[%s][%d][%s]", "encryptAES", 0x1f, "Encrypt Error");
        return NULL;
    }

    srcLen = paddedLen * 2 + 1;
    DNFFLib::Binary2Hex(reinterpret_cast<unsigned char*>(enc), paddedLen, outHex);
    outHex[srcLen] = '\0';
    return outHex;
}

}  // namespace ARAD_CRYPT

#include "Arad_Misc/limitValue.inc"
{
    // ORIG：std::max<int>(&value,&minValue) → std::min<int>(&maxValue)
    return std::min(std::max(value, minValue), maxValue);
}

#include "Arad_Misc/uint32Time.inc"
{
    // ORIG：time→localtime；tm_year=buf[0]-1900, tm_mon=buf[1]-1,
    // tm_mday=buf[2], tm_hour=buf[3], min/sec=0，mktime 落表。
    time_t now = time(static_cast<time_t*>(NULL));
    struct tm* tmv = localtime(&now);
    tmv->tm_year = *timeBuf - 1900;
    tmv->tm_mon = timeBuf[1] - 1;
    tmv->tm_mday = timeBuf[2];
    tmv->tm_hour = timeBuf[3];
    tmv->tm_min = 0;
    tmv->tm_sec = 0;
    mktime(tmv);
}

unsigned int inet_a2n(const char* addrText)
{
    // ORIG：空串/解析失败(-1) → 0；否则 inet_addr 结果。
    if (addrText == NULL) {
        return 0;
    }
    in_addr_t addr = inet_addr(addrText);
    if (addr == static_cast<in_addr_t>(-1)) {
        return 0;
    }
    return addr;
}

// [L31 round3 判 N · N82 族 / AradMisc-E3~E4 2026-09-11 · DIFF(58)→**NEAR** 85/85 gap0]
//   ORIG 0x08197a4d（sz 0x121 = 85 条）指令形（逐段取证据）：
//     ① 0x08197a68 `memset(lea -0x1c,0,0x10)` — 清一块**16B 独立区**（lea 一步到位）；
//     ② 0x08197a76 `mov 0xc(%ebp),%eax; mov %eax,-0x18` — in_addr **副本 spill**，
//        两次 inet_ntoa（0x08197a9b 判空 / 0x08197afd 取串）均从 -0x18 加载；
//     ③ 0x08197a79 起 4×`movl $0x0,{-0x2c,-0x28,-0x24,-0x20}` — buf 的**16B 初始化器**
//        （4×movl 直对 ebp 槽），且其**位置在 memset/副本之后** ⇒ 源码里该 16B 数组的
//        声明/初始化在副本之后。
//   ✅ 落地形（85 条 = ORIG，NEAR）：`char tmp[0x10]; memset(tmp,...)` + `in_addr inaddr = addr;`
//      + `char buf[0x10] = {0};` + 两次 `inet_ntoa(inaddr)` + `strncpy(buf,...,0x10)`。
//      `char buf[0x10] = {0};`（16B 初始化器）是 4×movl 的唯一可复现来源。
//   ⛔ 负结论（勿再试）：
//      · `char buf[0x20] = {0};`（32B 单一初始化器）⇒ 我方 **106 条**（gap −21）：
//        4.4 走 inline-memset 阶梯（`mov $0x20,%ebx; mov $0x0,%edx; and $0x2,%ecx; mov %dx,(%eax); …`），
//        `= {}`（值初始化）同形；32B 初始化器**不产** ORIG 的 4×movl 分段形。
//      · `&buf[0x10]` 与 `buf + 0x10` 等价（均 lea+add 两步，ORIG 的 lea 一步来自该 16B 区
//        自身即为数组起点）。
//      · 残余 NEAR 差：ORIG in_addr 副本(-0x18)与 memset 目标区(-0x1c)同槽、buf 落 -0x2c；
//        我方 inaddr(-0x20)/tmp(-0x1a)/buf(-0x30) 各差 2~4B（GCC 同作用域不复用槽），
//        助记符逐行一致、仅 disp 不同。
//      · laneE1 旧读数「补副本 + 第二次 call 零收益」系假 DIFF 口径（game_func_report 不带 -r）
//        ⇒ 该二旋钮实为**有效**，已落地。
std::string inet_n2a(struct in_addr addr)
{
    // ORIG：16B 块清零后 inet_ntoa→strncpy(buf,..,0x10)；null → 空串（`std::string("")`）。
    // [P6AuditOldN-S1 2026-09-11 负结论 P339/槽复用] 试「把 tmp 包进块作用域提前结束
    //   生命期」以复现 ORIG tmp@-0x1c 与 inaddr@-0x18 的槽重叠 ⇒ 实测 85/85 gap 0
    //   不变、槽改判到 tmp@-0x30/inaddr@-0x10/buf@-0x20 仍不成对（4.4 不复用同作用域
    //   兄弟块槽）⇒ 已回滚，维持 N82 槽族封顶。
    char tmp[0x10];
    std::memset(tmp, 0, 0x10);
    // ORIG 0x08197a9b / 0x08197afd：**两次独立** inet_ntoa（第一次结果仅判空、无具名指针槽），
    // 两次都从 in_addr 副本槽加载 ⇒ 具名副本 + 两处直呼。
    in_addr inaddr = addr;
    char buf[0x10] = {0};
    if (inet_ntoa(inaddr) == NULL) {
        return std::string("");
    }
    strncpy(buf, inet_ntoa(inaddr), 0x10);
    return std::string(buf);
}

namespace {

// 占位注释：ARAD::encryptAES 独立实现见下（不设 helper，ORIG 两处独立指令体）。

}  // namespace

char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    // ORIG 0x0819e997：与 ARAD_CRYPT::encryptAES 指令体相同，仅 logFormat 文案不同
    // （localjapan/Arad_OS.cpp、funcName 与 `%s` 实参均用宏、0x52）。
    // [AradPrett 2026-09-11 · A 类落地] ORIG 0x08ba9a20 size 0x31（串
    // `char* ARAD::encryptAES(CRijndael*, char*, char*)`）：本函数两处手写串改
    // `__PRETTY_FUNCTION__` 后 `_ZZN4ARAD10encryptAESEP9CRijndaelPcS2_E19__PRETTY_FUNCTION__`
    // 与 ORIG 逐字节 SAME。
    // [AradMisc-E10 2026-09-11 · DIFF(91)→**NEAR** 89/89 gap0] 与 ARAD_CRYPT::encryptAES
    //   同套六旋钮（ORIG 0x0819e9b5/0x0819e9fe/0x0819ea0c/0x0819ecc4/0x0819ecc8 同形）：
    //   `int srcLen`（有符号 idiv 段）/ `int paddedLen = 0;` / 嵌套 `if (blockSize >= 0)`
    //   / 内层 `if (mod != 0)` then 后置 / `if (result != 0x6fffffff) { log; return NULL; }`
    //   / hexLen 复用 srcLen 槽。残余 NEAR 同 encA：Encrypt 实参装载寄存器选择。
    char enc[132];
    int srcLen = strlen(inData);
    int paddedLen = 0;
    int blockSize = cipher->IMethod::GetBlockSize(NULL);
    if (blockSize >= 0) {
        if (srcLen % blockSize != 0) {
            paddedLen = (srcLen / blockSize + 1) * blockSize;
        } else {
            paddedLen = srcLen;
        }
    } else {
        paddedLen = srcLen;
    }
    if (blockSize >= 0) {
        cipher->IMethod::Pad(inData, srcLen, NULL);
    }
    int result = cipher->Encrypt(inData, enc, paddedLen);   // P6AuditOldN-S1 试形负结论同 AC 版
    if (result != 0x6fffffff) {
        LogManager::logFormat(
            1, "localjapan/Arad_OS.cpp", __PRETTY_FUNCTION__, 0x52,
            "[%s][%d][%s]", __PRETTY_FUNCTION__, 0x52,
            "Encrypt Error");
        return NULL;
    }

    srcLen = paddedLen * 2 + 1;
    DNFFLib::Binary2Hex(reinterpret_cast<unsigned char*>(enc), paddedLen, outHex);
    outHex[srcLen] = '\0';
    return outHex;
}

// [b28d-L1] N82 封顶：ORIG 0x0819eaf8；P167 CLEAR（3/3）⇒ 语义完成。
// [AradMisc-E1/E2 2026-09-11 · DIFF→**AE** 70/70 gap0] 残余为 cMyTrace 实参装载/帧槽形态差，
//   两旋钮落地：① 去 `raw` 具名指针（ORIG 0x0819eb01 起一律 `mov 0xc(%ebp),%eax` 基址访问，
//   无 raw 槽）⇒ 70→66；② 临时对象直呼 `cMyTrace(...)(...)`（ORIG 0x0819eb73 ctor 调用位于
//   **全部**复杂实参求值之后、装参之前；具名 `trace` 形实测多 4 条）⇒ 66→70 = ORIG。
#include "Arad_Misc/write_incorrect_item_info.inc"
                               const char* invenType, int errorLine, int slotNo)
{
    // ORIG：cMyTrace(funcName,0x106,5)(fmt,...)；字段映射按 Inven_Item.h 布局：
    //   Seal=+0x00 / ItemType=+0x01 / ItemID=+0x02(m_addInfo) / ItemAt=GetItemAttr&0xff /
    //   add_info=+0x07(m_addInfo2) / endurance=+0x0b / extend_info=+0x0d(m_fieldd) /
    //   amp_opt=+0x11(`*(const unsigned*)((char*)&item + 0x11)`：ORIG 0x0819eb04 用
    //   `add $0x11,%eax; mov (%eax),%edi` 两段形 = cast 口径，非成员直访 disp 形)。
    cMyTrace(
        __PRETTY_FUNCTION__,
        0x106, 5)("charac_no[%u],Seal[%d],ItemType[%d],ItemID[%u],ItemAt[%d],add_info[%d],endurance[%d],extend_info[%u],amp_opt[%d] - inven_type[%s] slot_no(%d), error_line(%d) \n",
                  characNo,
                  static_cast<unsigned int>(static_cast<unsigned char>(item.m_field0)),
                  static_cast<unsigned int>(item.m_field1),
                  item.m_addInfo,
                  static_cast<unsigned int>(item.GetItemAttr()) & 0xff,
                  item.m_addInfo2,
                  static_cast<unsigned int>(item.m_fieldb),
                  item.m_fieldd,
                  *reinterpret_cast<const unsigned int*>(&item.m_amp),
                  invenType, slotNo, errorLine);
}

}  // namespace ARAD


namespace ARAD {

}  // namespace ARAD