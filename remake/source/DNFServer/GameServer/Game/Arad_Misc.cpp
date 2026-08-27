// ============================================================================
// df_game_r 还原 —— ARAD 域杂项工具（localjapan/Arad_OS.cpp、Arad_Function_Set.cpp 域）
//
// ORIG 证据（docs/class_func_reports/ARAD/{limitValue,uint32Time,inet_a2n,inet_n2a,
//   encryptAES,write_incorrect_item_info,send_mail}.md + ARAD__ARAD_CRYPT/encryptAES.md，
//   nm -S --defined-only df_game_r）：
//   _ZN4ARAD10limitValueEiii                       T 0x081978b2
//   _ZN4ARAD10uint32TimeEPs                        T 0x081979c0
//   _ZN4ARAD8inet_a2nEPKc                          T 0x08197b71
//   _ZN4ARAD8inet_n2aE7in_addr                     T 0x08197a4d
//   _ZN4ARAD10encryptAESEP9CRijndaelPcS2_          T 0x0819e997
//   _ZN4ARAD10ARAD_CRYPT10encryptAESEP9CRijndaelPcS3_ T 0x08184abc
//   _ZN4ARAD25write_incorrect_item_infoEjRK10Inven_ItemPKcii T 0x0819eaf8
//   _ZN4ARAD15Arad_Dispatcher13dispathc_codeEv     T 0x0818e898
//   _ZN10ARAD_DATAS12AradMailInfoC1Ev              W 0x081a3ff2
//   _ZN4ARAD9send_mailERN10ARAD_DATAS12AradMailInfoE T 0x08196b03
//
// 注：ARAD::encryptAES 与 ARAD::ARAD_CRYPT::encryptAES 在 ORIG 为两份相同指令体
// （均 0x14d 字节，独立编译），此处按 ORIG 各自完整实现。
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

int limitValue(int value, int minValue, int maxValue)
{
    // ORIG：std::max<int>(&value,&minValue) → std::min<int>(&maxValue)
    if (value < minValue) {
        value = minValue;
    }
    if (maxValue < value) {
        value = maxValue;
    }
    return value;
}

void uint32Time(short* timeBuf)
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

// 两份 encryptAES 的共同本体（ORIG 两处独立同型指令体，本文件以 helper 承载一次）。
char* encryptAES_impl(CRijndael* cipher, char* inData, char* outHex)
{
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

}  // namespace

char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    return encryptAES_impl(cipher, inData, outHex);
}

namespace ARAD_CRYPT {

char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    // ORIG 与自由函数版为两份等价指令体，语义收敛同一实现。
    return encryptAES_impl(cipher, inData, outHex);
}

}  // namespace ARAD_CRYPT

void write_incorrect_item_info(unsigned int characNo, const Inven_Item& item,
                               const char* invenType, int errorLine, int slotNo)
{
    // ORIG：cMyTrace(funcName,0x106,5)(fmt,...)；字段映射按 Inven_Item.h 布局：
    //   Seal=+0x00 / ItemType=+0x01 / ItemID=+0x02(m_addInfo) / ItemAt=GetItemAttr&0xff /
    //   add_info=+0x07(m_addInfo2) / endurance=+0x0b / extend_info=+0x0d(m_fieldd) /
    //   amp_opt=+0x11(stAmplifyOption_t 首 dword)。
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    cMyTrace trace(
        "void ARAD::write_incorrect_item_info(unsigned int, const Inven_Item&, const char*, int, int)",
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

int send_mail(ARAD_DATAS::AradMailInfo& info)
{
    // ORIG 0x08196b03：Acquire(stream)→CStreamGuard(true)→<<0x80(msgcode)/<<(uid=-1)；
    // sig=GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>() 按字节偏移填充，异常路径由
    // CStreamGuard 栈展开析构（对应 ORIG catch→D1→_Unwind_Resume 形态）。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_Function_Set.cpp", 0x3a);
    CStreamGuard guard(stream, true);
    try {
        guard << 0x80;
        guard << -1;
        SIG_MAILBOX_SEND_NEW_MAIL* sig =
            guard.GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
        char* base = reinterpret_cast<char*>(sig->m_data);
        *(reinterpret_cast<unsigned int*>(base + 0x22)) = info.m_fieldC;
        *(reinterpret_cast<unsigned int*>(base + 0x1e)) = info.m_field10;
        *(base + 0x15f) = static_cast<unsigned char>(info.m_isSystemMail);
        *(reinterpret_cast<unsigned int*>(base + 0x15b)) = info.m_field8;
        *(reinterpret_cast<unsigned int*>(base + 0x26)) = info.m_field4;
        *(reinterpret_cast<unsigned short*>(base + 0x59)) =
            static_cast<unsigned short>(info.m_field14);
        std::memset(base + 0x5b, 0, 0x100);
        strncpy(base, info.m_name, 0x1d);
        if (info.m_content != NULL) {
            size_t len = strlen(info.m_content);
            strncpy(base + 0x5b, info.m_content, len);
        }
        ARAD_DATAS::AradMailItemInfo* item = &info.m_item;
        *(base + 0x2a) = static_cast<char>(item->m_flag);
        *(reinterpret_cast<unsigned int*>(base + 0x2b)) = item->m_fieldC;
        *(reinterpret_cast<unsigned int*>(base + 0x2f)) = item->m_field10;
        *(reinterpret_cast<unsigned short*>(base + 0x34)) = item->m_fieldA;
        *(base + 0x44) = '\0';
        *(reinterpret_cast<unsigned short*>(base + 0x45)) = 0;
        *(base + 0x58) = '\0';
        *(base + 0x56) = '\0';
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    } catch (...) {
        throw;
    }
    return 1;
}

}  // namespace ARAD