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
    unsigned char enc[132];
    size_t srcLen = strlen(inData);
    size_t paddedLen;
    int blockSize = cipher->IMethod::GetBlockSize(NULL);
    if (blockSize < 0) {
        paddedLen = srcLen;
    } else if (static_cast<int>(srcLen) % blockSize == 0) {
        paddedLen = srcLen;
    } else {
        paddedLen = (srcLen / blockSize + 1) * blockSize;
    }
    if (blockSize >= 0) {
        cipher->IMethod::Pad(inData, static_cast<int>(srcLen), NULL);
    }
    int result = cipher->Encrypt(inData, reinterpret_cast<char*>(enc), paddedLen);
    if (result == 0x6fffffff) {
        size_t hexLen = paddedLen * 2 + 1;
        DNFFLib::Binary2Hex(enc, static_cast<int>(paddedLen), outHex);
        outHex[hexLen] = '\0';
        return outHex;
    }

    LogManager::logFormat(
        1, "localjapan/Arad_Crypt.cpp", "encryptAES", 0x1f,
        "[%s][%d][%s]", "encryptAES", 0x1f, "Encrypt Error");
    return NULL;
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

// [L31 round3 判 N · N82 族] ORIG 0x08197a4d（sz 0x121 = 85 条）：清零形为
//   4×`movl $0x0,{-0x2c..-0x20}`（前 16B）+ `memset(-0x1c,0,0x10)`（后 16B），
//   且 `inet_ntoa` 两次独立调用。实测两形均不可翻：
//   ①`char buf[0x20] = {0};` + 两次 inet_ntoa ⇒ 我方 93 条 / clean 差异 **27→87 行（反劣）**；
//   ②原形（分段 memset + 具名 text 缓存）⇒ 58 条 / 27 行。
//   ⇒ 4.4 对 0x20 数组 `={0}` 不产 4×movl 分段形（clr_l 恒 orig=4 our=0），属跨版本初始化器展开差，
//   与 P312/P202 反向（那两族的 ORIG 是 rep stos 慢路）；回滚保留原形。
std::string inet_n2a(struct in_addr addr)
{
    // ORIG：buf[0x20] 清零后 inet_ntoa→strncpy(buf,..,0x10)；null → 空串。
    char buf[0x20];
    std::memset(buf + 0x10, 0, 0x10);
    const char* text = inet_ntoa(addr);
    if (text == NULL) {
        return std::string();
    }
    strncpy(buf, text, 0x10);
    return std::string(buf);
}

namespace {

// 占位注释：ARAD::encryptAES 独立实现见下（不设 helper，ORIG 两处独立指令体）。

}  // namespace

char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    // ORIG 0x0819e997：与 ARAD_CRYPT::encryptAES 指令体相同，仅 logFormat 文案不同
    // （localjapan/Arad_OS.cpp、"char* ARAD::encryptAES(CRijndael*, char*, char*)"、0x52）。
    unsigned char enc[132];
    size_t srcLen = strlen(inData);
    size_t paddedLen;
    int blockSize = cipher->IMethod::GetBlockSize(NULL);
    if (blockSize < 0) {
        paddedLen = srcLen;
    } else if (static_cast<int>(srcLen) % blockSize == 0) {
        paddedLen = srcLen;
    } else {
        paddedLen = (srcLen / blockSize + 1) * blockSize;
    }
    if (blockSize >= 0) {
        cipher->IMethod::Pad(inData, static_cast<int>(srcLen), NULL);
    }
    int result = cipher->Encrypt(inData, reinterpret_cast<char*>(enc), paddedLen);
    if (result == 0x6fffffff) {
        size_t hexLen = paddedLen * 2 + 1;
        DNFFLib::Binary2Hex(enc, static_cast<int>(paddedLen), outHex);
        outHex[hexLen] = '\0';
        return outHex;
    }

    LogManager::logFormat(
        1, "localjapan/Arad_OS.cpp", "char* ARAD::encryptAES(CRijndael*, char*, char*)", 0x52,
        "[%s][%d][%s]", "char* ARAD::encryptAES(CRijndael*, char*, char*)", 0x52,
        "Encrypt Error");
    return NULL;
}

// [b28d-L1] N82 封顶：ORIG 0x0819eaf8；P167 CLEAR（3/3）⇒ 语义完成；残余为 cMyTrace 实参装载/帧槽形态差（real diff 66）。
#include "Arad_Misc/write_incorrect_item_info.inc"
                               const char* invenType, int errorLine, int slotNo)
{
    // ORIG：cMyTrace(funcName,0x106,5)(fmt,...)；字段映射按 Inven_Item.h 布局：
    //   Seal=+0x00 / ItemType=+0x01 / ItemID=+0x02(m_addInfo) / ItemAt=GetItemAttr&0xff /
    //   add_info=+0x07(m_addInfo2) / endurance=+0x0b / extend_info=+0x0d(m_fieldd) /
    //   amp_opt=+0x11(stAmplifyOption_t 首 dword)。
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x106, 5);
    trace("charac_no[%u],Seal[%d],ItemType[%d],ItemID[%u],ItemAt[%d],add_info[%d],endurance[%d],extend_info[%u],amp_opt[%d] - inven_type[%s] slot_no(%d), error_line(%d) \n",
          characNo,
          static_cast<unsigned int>(raw[0x00]),
          static_cast<unsigned int>(raw[0x01]),
          static_cast<unsigned int>(item.m_addInfo),
          static_cast<unsigned int>(item.GetItemAttr()) & 0xff,
          item.m_addInfo2,
          static_cast<unsigned int>(item.m_fieldb),
          *reinterpret_cast<const unsigned int*>(raw + 0x0d),
          *reinterpret_cast<const unsigned int*>(raw + 0x11),
          invenType, slotNo, errorLine);
}

}  // namespace ARAD


namespace ARAD {

}  // namespace ARAD