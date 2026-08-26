// Arad_Function_Set.cpp —— 原始工程 localjapan/Arad_Function_Set.cpp 的复刻 TU。
// 收录 ARAD::PRINT_ERROR 三个静态错误上报函数与 ARAD::notifySystemMessage。
//   isEmpty       ORIG T 0x08197ba4 sz 0x59 _ZN4ARAD11PRINT_ERROR7isEmptyEPcP5CUsert
//   CheckItemLock ORIG T 0x08197bfd sz 0x59 _ZN4ARAD11PRINT_ERROR13CheckItemLockEPcP5CUsert
//   delete_item   ORIG T 0x08197c56 sz 0x59 _ZN4ARAD11PRINT_ERROR11delete_itemEPcP5CUsert
//   notifySystemMessage ORIG T 0x08197d00 sz 0xb5 _ZN4ARAD19notifySystemMessageEP5CUserPc
//
// PRINT_ERROR 三函数体完全同构：取 user 当前角色编号后经 LogManager::logFormat
// 上报（level=1，file="localjapan/Arad_Function_Set.cpp"，__PRETTY_FUNCTION__
// 静态串由编译器自动生成，对应 ORIG .rodata @0x8b9d8e0 等）。行号常量
// 0x29d/0x2a2/0x2a7 为 ORIG 硬编码源行。
// notifySystemMessage 无独立报告，按 objdump 0x8197d00 转写【推断】：
//   user/msg 任一为空直接返回；否则 PacketGuard 组包
//   GameWorld::make_packet_chat_msg(guard,user,msg,len,type=0,NOTIPACKET=0xc)
//   后 CUser::Send(guard) 发给本人（异常路径 PacketGuardD1+Unwind_Resume 由
//   编译器生成）。
// 报告：docs/class_func_reports/ARAD__PRINT_ERROR/{isEmpty,CheckItemLock,delete_item}.md

#include "Arad_DataManager.h"

#include "CUser.h"
#include "CUserCharacInfo.h"    // getCurCharacNo（ORIG W 0x080cbc4e）
#include "GameWorld.h"          // G_GameWorld / make_packet_chat_msg
#include "LogManager.h"         // logFormat（ORIG T 0x08ad3c0a）
#include "PacketGuard.h"

#include <cstring>

namespace ARAD
{

// ---- PRINT_ERROR::isEmpty @ 0x08197ba4 ------------------------------------
void PRINT_ERROR::isEmpty(char* tag, CUser* user, unsigned short slot)
{
    register unsigned int characNo __asm__("eax") =
        (unsigned int)user->getCurCharacNo();
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        "void ARAD::PRINT_ERROR::isEmpty(char*, CUser*, short unsigned int)",
        0x29d,
        "%s : isEmpty() [slot:%d] [charac_no:%d]",
        tag, (unsigned int)slot, characNo);
}

// ---- PRINT_ERROR::CheckItemLock @ 0x08197bfd -------------------------------
void PRINT_ERROR::CheckItemLock(char* tag, CUser* user, unsigned short slot)
{
    register unsigned int characNo __asm__("eax") =
        (unsigned int)user->getCurCharacNo();
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        "void ARAD::PRINT_ERROR::CheckItemLock(char*, CUser*, short unsigned int)",
        0x2a2,
        "%s : CheckItemLock() fail [slot:%d] [charac_no:%d]",
        tag, (unsigned int)slot, characNo);
}

// ---- PRINT_ERROR::delete_item @ 0x08197c56 ---------------------------------
void PRINT_ERROR::delete_item(char* tag, CUser* user, unsigned short slot)
{
    register unsigned int characNo __asm__("eax") =
        (unsigned int)user->getCurCharacNo();
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        "void ARAD::PRINT_ERROR::delete_item(char*, CUser*, short unsigned int)",
        0x2a7,
        "%s : delete_item() [slot:%d] [charac_no:%d]",
        tag, (unsigned int)slot, characNo);
}

// ---- notifySystemMessage @ 0x08197d00（objdump 转写【推断】）-----------------
void notifySystemMessage(CUser* user, char* msg)
{
    if (user == 0)          // 0x8197d08: cmpl $0x0,0x8(%ebp) → je ret
        return;
    if (msg == 0)           // 0x8197d12: cmpl $0x0,0xc(%ebp) → je ret
        return;

    int len = static_cast<int>(strlen(msg));   // 0x8197d22 strlen

    PacketGuard packet;                        // -0x18 栈对象
    // type=0、notify=(ENUM_NOTIPACKET)0xc（ORIG movl $0xc,0x18(%esp)）
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, len, (char)0, (ENUM_NOTIPACKET)0xc);
    user->Send(packet);
}

}  // namespace ARAD
