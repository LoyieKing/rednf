#ifndef DNF_GAME_ARAD_LOGINSECURITY_H_
#define DNF_GAME_ARAD_LOGINSECURITY_H_

// ============================================================================
// df_game_r 还原 —— ARAD 登录安全域（Wave6 ARAD 残余清零批，
// 独占 TU source/DNFServer/GameServer/Game/ARAD_LoginSecurity.cpp）。
//
// 权威报告：
//   docs/class_func_reports/ARAD__Arad_Dispatcher_Login_Impl/
//   docs/class_func_reports/ARAD__PUNISH_CHECK/
//   docs/class_func_reports/ARAD__PVP_CHANNEL/
//   docs/class_func_reports/ARAD__AradWhiteAccount/
//   ARAD::pvp_grade_check 无独立报告 —— 按 ORIG objdump 0x0819ebde 反汇编转写
//   （正文内标【objdump】）。
//
// 本头为唯一声明点；实现全部落在 ARAD_LoginSecurity.cpp。
// ============================================================================

#include <set>
#include <vector>

#include "Arad_Dispatcher.h"     // ARAD::Arad_Dispatcher 基类（vtable 槽序权威）

class SIG_LOGIN_DATA;
class CRijndael;
class CUser;
#include "DB_Login.h"           // DB_Login::tagDNFEventEntry 完整类型

namespace ARAD
{

// ----------------------------------------------------------------------------
// ARAD::ARAD_CRYPT —— ORIG T _ZN4ARAD10ARAD_CRYPT10encryptAESEP9CRijndaelPcS3_
// @0x08184abc（localjapan 加密域；实现属其它批次，此处仅声明接口）。
// ----------------------------------------------------------------------------
namespace ARAD_CRYPT
{
char* encryptAES(CRijndael* cipher, char* plain, char* out);  // ORIG T 0x08184abc（Ghidra C 返 char*）
}  // namespace ARAD_CRYPT

// ----------------------------------------------------------------------------
// ARAD::DATABASE::LOGIN_QUERY —— 登录 DB 存储过程包装（ORIG 全 T 符号，
// 定义尚未还原属其它批次；本 TU 按 ORIG 调用契约声明并使用）：
//   arad_sp_select_account_info        0x0818b706
//   arad_sp_select_punish_user_info    0x0818bd67
//   arad_sp_select_punish_second_user  0x0818bfb0
//   arad_sp_convert_pass_pad_info      0x0818c0e0
//   arad_sp_update_fail_record         0x0818c6c4
//   arad_sp_delete_member_mouspass     0x0818c609
//   arad_sp_update_history             0x0818c76f
//   arad_sp_load_event_entry           0x0818c8b8
//   arad_sp_select_member_brodcast     0x0818ca69
//   arad_sp_select_member_premium      0x0818cc74
//   arad_sp_check_goblin_reward        0x0818c4f3
// ----------------------------------------------------------------------------

namespace DATABASE
{

struct LOGIN_STATUS_TRAIT_0x21_;  // arad_sp_select_account_info 的 out 参数仅为
                                  // char[0x84] 明文口令缓冲（TU 内局部化）

namespace LOGIN_QUERY
{

int arad_sp_select_account_info(SIG_LOGIN_DATA* data, char* pwdBuf);
int arad_sp_select_punish_user_info(SIG_LOGIN_DATA* data);
int arad_sp_select_punish_second_user(SIG_LOGIN_DATA* data);
int arad_sp_convert_pass_pad_info(SIG_LOGIN_DATA* data);
int arad_sp_update_fail_record(SIG_LOGIN_DATA* data);
int arad_sp_delete_member_mouspass(SIG_LOGIN_DATA* data);
int arad_sp_update_history(SIG_LOGIN_DATA* data, int flag);
int arad_sp_load_event_entry(
    SIG_LOGIN_DATA* data,
    std::vector<DB_Login::tagDNFEventEntry>& out);
void arad_sp_select_member_brodcast(SIG_LOGIN_DATA* data,
                                    DB_Login::tagDNFEventEntry& entry,
                                    int serverGroup);
void arad_sp_select_member_premium(
    SIG_LOGIN_DATA* data,
    DB_Login::tagDNFEventEntry& entry,
    std::vector<int>& dstA,
    std::vector<int> srcB);
int arad_sp_check_goblin_reward(SIG_LOGIN_DATA* data, char* day);

}  // namespace LOGIN_QUERY

}  // namespace DATABASE

// ----------------------------------------------------------------------------
// ARAD::PUNISH_CHECK —— 处罚检查表驱动（全部静态方法；
// ORIG 调用方以cdecl传入 SIG_LOGIN_DATA* 作第一参数，与静态成员 ABI 一致）。
// 复核结果落位：SIG_LOGIN_DATA+0xb8 = 当前登录错误码槽。
// ----------------------------------------------------------------------------
class PUNISH_CHECK
{
public:
    // HUB：无条件将 +0x3a20 字节置 1（0x08196dbf）
    static void punish_check_by_HUB(SIG_LOGIN_DATA* data, int type);
    // RES：ORIG 纯 ret（0x08196dce）
    static void punish_check_by_RES(SIG_LOGIN_DATA* data, int type);
    // WEB：0x08196d1a —— flag==1 时 errSlot=4，arg==0 再改 1001(0x3e9)；返回 0/1
    static int punish_check_by_WEB(SIG_LOGIN_DATA* data, long arg, int flag);
    // SYS：0x08196d88 —— 同 WEB，arg==0 时改 1002(0x3ea)
    static int punish_check_by_SYS(SIG_LOGIN_DATA* data, long arg, int flag);
    // ARS：0x08196d51 —— 同 WEB，arg==0 时改 1003(0x3eb)
    static int punish_check_by_ARS(SIG_LOGIN_DATA* data, long arg, int flag);
};

// ----------------------------------------------------------------------------
// ARAD::PVP_CHANNEL —— 渠道类型判定工具集（全部静态；GameWorld 渠道号：
// 4/8 = winpoint pvp，7 = 通用判定分支，bitmask 0x1e700 为"综合 pvp 渠道"集合）。
// ----------------------------------------------------------------------------
class PVP_CHANNEL
{
public:
    static int is_integrated_pvp_channel();      // 0x0819763c
    static int is_broadcast_channel();            // 0x08197684
    static int is_all_pvp_channel();              // 0x081976c7
    static int is_winpoint_pvp_channel();         // 0x0819770a
    static int is_common_channel();               // 0x0819773b
    // 非 all-pvp 渠道时清零 SIG_LOGIN_DATA+0x3a38（short 槽）；恒返回 1。
    // 0x08197781
    static int check_Login_server_group(SIG_LOGIN_DATA* data);
};

// ----------------------------------------------------------------------------
// ARAD::pvp_grade_check —— pvp 进场资格校验（【objdump】0x0819ebde，全 TU
// 唯一 ORIG 符号 _ZN4ARAD15pvp_grade_checkEP5CUser，size 0x1a6）。
// ----------------------------------------------------------------------------
bool pvp_grade_check(CUser* user);

// ----------------------------------------------------------------------------
// ARAD::AradWhiteAccount —— 登录白名单账号集合（sizeof = std::set<unsigned>
// 完整继承；ctor 即加载固定名单；dtor 析构 set 链）。
// ----------------------------------------------------------------------------
class AradWhiteAccount : public std::set<unsigned int>
{
public:
    AradWhiteAccount();          // C1/C2 0x081ab518：base-set 构造后 Load()
    ~AradWhiteAccount();         // D1/D2 0x081ab55a（~set 自动链条）

    int Load();                  // 0x08ab5f8：插入 0x27fa70/0x27fa74/0x27fa63，返1
    bool IsWhiteAccount(unsigned int accId);  // 0x081ab59c：find != end
};

// ----------------------------------------------------------------------------
// ARAD::Arad_Dispatcher_Login_Impl —— 登录库分派器（派发码 1）。
// vtable @0x08b98fd0：[D1][D0][db_dispatch 0x0818ef4c][inter_dispatch 0x0818ef42]
// ----------------------------------------------------------------------------
class Arad_Dispatcher_Login_Impl : public Arad_Dispatcher
{
public:
    Arad_Dispatcher_Login_Impl();   // C1/C2 0x0818eed0：base(code=1)
    ~Arad_Dispatcher_Login_Impl();  // D1/D2 0x0818eef4 / D0 0x0818ef24

    // 虚槽 2（db_dispatch）。0x27d 指令，详见 cpp。
    virtual int db_dispatch(int code, int userIdx, Stream* stream);
    // 虚槽 3（inter_dispatch）。ORIG 0x0818ef42 纯 ret 恒返 0（忠实空体）。
    virtual int inter_dispatch(CUser* user, char* body, int len);

    // —— 登录主流程子步骤（供 db_dispatch 内部链路）——
    // SSO 会话校验 + GM 口令核对（0x0818f746）
    int account_password_check(SIG_LOGIN_DATA* data, char* clientPwd);
    // 处罚记录遍历表驱动（0x0818f970）
    int punish_user_check(SIG_LOGIN_DATA* data);
    // 二级口令(GoblinPad) 校验链（static；0x0818f4a6）
    static int pad_certifiaction_check(SIG_LOGIN_DATA* data);
    // premium/event 入账分发（0x0818f1ca）
    int premium_info_set(
        SIG_LOGIN_DATA* data,
        std::vector<DB_Login::tagDNFEventEntry>& entries,
        int serverGroup);
};

}  // namespace ARAD

#endif  // DNF_GAME_ARAD_LOGINSECURITY_H_
