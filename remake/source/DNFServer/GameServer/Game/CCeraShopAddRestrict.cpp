// ============================================================================
// df_game_r 还原 —— CerashopAddRestrict / CItemLimitEdition / CeraShop 事件
// （G4 商城/交易批次）
// 参照 docs/class_func_reports/CerashopAddRestrict*.md、CItemLimitEdition*.md、
// CCeraShopBonusItemEvent.md + ORIG 反汇编。
// 用户限购数据位于 CUser +0x8d1e4（4 张 map）。
// CEventCeraShopRewardPoint 已拆分至 CEventCeraShopRewardPoint.h/cpp
// （CEventBase 权威声明见 CGuildServerProxy.h，本文件内仅保留
// CCeraShopBonusItemEvent 及其局部基类）。
// ============================================================================
// b28d-solo-CCeraShopAddRestrict 判词汇总（本 TU 9 件，ORIG 地址见各函数注释）：
//   Manager::C1 0x080e09bc AE：P299/CWG434——CUser.h ScriptLoader 删用户空 ctor
//     使 m_loader 的 value-init 前置零初始化（5×`movl $0`）复现（/tmp/probe_sl.cpp）。
//   InputBuyInfoDetail 0x080e1196 AE：m_count int→unsigned（P211档③，CUser.h 代落）
//     + pair 临时直构（P278）+ `new paramDaily()` value-init（P90/P299）。
//   readTime 0x08abf164 AE：`return 1` 移入 try 块（P229/N104 正常路径尾物化）。
//   LoadProcess 0x080e142a AE：`new paramDaily()` + SIG `m_items` 具名数组视图
//     （P291，SigTypes_B.h 字段化）+ m_count unsigned。
//   FindUserInfoDetail 0x080e0a43 NEAR→N 封顶：P196 比较方向 + m_count unsigned 已修；
//     残余 N95 槽基差 8 + N86 insert sret 槽位；p167 CLEAR 129=129。
//   InputBuyInfo 0x080e129a AE：m_result 具型化 paramDaily*（P211档③ LHS-first，
//     CUser.h 代落）+ switch（P251/P261）。
//   LoadScript 0x08abf3cc AE：`while(true){if(!ScanType)break;}`（P283 头部条件）
//     + 3 个 dead bool 预清（P232）。
//   makeSyncPacket 0x080e0f3a AE：P274 累加异型 cast + 去 buf 具名指针（P183）
//     + empty 正极性 else 形（P292）+ P223 尾空标签 nop 岛。
//   LoadScriptDaily 0x08abf4de N 封顶：P292 readTime 返回 bool、P214③ t1/t2={0}、
//     P223 双岛 goto、P167 operator==（printf 岛布局）、P185 printf 原文；残余
//     N95 槽基差 8 + N93 块内槽序 + N7 寄存器中转（edi/eax）；p167 CLEAR 154=154。
//   注：__static_initialization_and_destruction_0ii 为 TU 级静态初始化（非清单件）。

#include <ctime>
#include <cstring>
#include <map>
#include <string>
#include "RDARScriptStringManager.h"
#include "CGameManager.h"
#include "CInventory.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "Arad_DataManager.h"
#include "CItemLimitEdition.h"
#include "CEnvironment.h"   // G_CEnvironment()（GetCalcDay 读 env+0x37c）
#include "LogManager.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
// DNFLexWrapper.h 与本文件 include 链冲突，ScanTypeOrInt 用本地声明
//（定义见 DNFLexWrapperHelpers.cpp，ORIG 0x088bceaf）。
int ScanTypeOrInt(bool& isType, std::string& s, bool& isInt, bool consume);

// DB 商城限购请求类（权威定义见 DB_CerashopAddRestrict.cpp，ORIG T @ 0x080e1ff6/0x080e17ba）。
class DB_LoadCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user);
};
class DB_SaveCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};


// ARAD::Singleton 模板定义已在 Arad_DataManager.h 提供（唯一）；此处不再定义。
// 供 ARAD::Singleton<CerashopAddRestrict::Manager>::Get() 隐式实例化，产出 ORIG 符号
// _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）。

// 全局 C++ 权威签名（由 DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）：
bool loadRDARScriptFile(const char* directory, const char* path);
bool ScanType(std::string& token, bool consume);
int ScanInt(bool* ok);
bool ScanStr(std::string* value);

// DB 请求类（定义于 DB_CerashopAddRestrict.cpp，ORIG 0x080e1ff6/0x080e17ba）。
// 全局作用域类；SAVE 参数类型为 CerashopAddRestrict::DBInputData。
class DB_LoadCerashopAddRestrictType;
class DB_SaveCerashopAddRestrictType;

namespace CerashopAddRestrict
{

// [b28c-4] ORIG 0x080e09bc 实形 = ScriptInterface::C1（写基类 vptr）→
// Singleton<Manager>::C1（空）→ 派生 vptr → 5 条 `mov 0x8(%ebp),%eax;
// movl $0x0,0x8|0xc|0x10|0x14|0x18(%eax)` → ScriptLoader::C1(this+4)。
// 该 5 条落在 map(+4) 的 _Rb_tree_impl 内部（color/parent/left/right/
// node_count），是 4.1 对成员 value-init 的**前置 zero-init**（P90/N77 族），
// 非 Manager 成员——原 5 个 int 成员系臆造，已由 Main 代落删除
// （硬证据：单例 obj nm -S size=0x1c = vptr4 + m_loader 0x18）。
// b28d-solo-CCeraShopAddRestrict: AE（27=27；P299/CWG434 由 CUser.h 删 ScriptLoader
// 用户空 ctor 代落）。
Manager::Manager()
    // [b28c-4] 试 value-init 形复现 ORIG 4.1 对 m_loader 的前置 zero-init
    //（P90/N77：4.4 按 CWG434 可能仍不 emit）。
    : m_loader()
{
}

// b28d-solo-CCeraShopAddRestrict: AE（83=83，仅 .o 重定位显示差；P283 头部条件 +
// P232 三个 dead bool 预清）。
int ScriptLoader::LoadScript()
{
    if (!loadRDARScriptFile("Etc/", "NewCashShop_Restrict.etc"))
        return -1;
    std::string section;
    // [b28d-solo P232] ORIG 0x08abf404 三条 `movb $0x0,-0xb|-0xa|-0x9(%ebp)` 是
    // 原码三个 dead bool 局部的初始化（section 因此落 -0x10 槽）；探针
    // /tmp/probe_bool.cpp f2 实测 4.4 同形（未使用亦不 DCE）。
    bool isAccountSection = false;
    bool isCharacSection = false;
    bool sectionHandled = false;
    // [b28d-solo P283/P164] ORIG 0x08abf3f9：循环头直落 ScanType（条件在头部），
    // 成功路径经 +0x46/+0x49/+0x4c 三个 nop 岛回跳；4.4 对 `while(cond)` 恒
    // 旋转成「body 在前、cond 在后」。`while(true){ if(!ScanType) break; ... }`
    // 复现头部条件 + `xor $1; test %al; jne 退出` 形。
    while (true) {
        if (!ScanType(section, true))
            break;
        if (section == "[account restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Account) < 0)
                return -1;
        } else if (section == "[character restrict]") {
            if (LoadScriptDaily(RestrictType::RestrictType_Charac) < 0)
                return -1;
        }
    }
    return 0;
}

// b28d-solo-CCeraShopAddRestrict: N 封顶（154=154、p167 CLEAR；残余 N95 槽基差 8 +
// N93 块内槽序 + N7 寄存器中转 edi/eax；语义闭合）。
int ScriptLoader::LoadScriptDaily(RestrictType::T type)
{
    // ORIG 0x08abf4de：首 token 用 ScanTypeOrInt(isType, s, ok, true) 解析
    // （支持类型名/整数），isType 或 !ok 即整函数返回 0；
    // 字段顺序 ipgNo, fieldA, fieldB, t1, t2, fieldC（fieldA/fieldB 在时间前）；
    // readTime 失败 → 返回 -1；find 命中（重复 ipgNo）→ printf + 返回 -1；
    // 末尾 m_infoMap.insert(make_pair(ipgNo, info))。
    std::string str;
    bool isType = false;
    bool ok = false;
    // [b28d-solo P232] ORIG 0x08abf4fd 第三个 dead bool（-0x25）预清零。
    bool dailyLoaded = false;
    while (true) {
        // [b28d-solo P214③] ORIG 0x08abf501 循环头 4 条 `movl $0,-0x4e|-0x4a|
        // -0x56|-0x52` = t1[4]/t2[4] 的 `={0}` 预清（每轮重清，8 字节各 2 dword）。
        short t1[4] = {0}, t2[4] = {0};
        // [b28d-solo P0] ORIG 0x08abf53e `mov %eax,-0x5c`（ScanTypeOrInt 结果直接
        // 落 ipgNo 槽，后续 printf/find/pair 全用该槽）；`int v0` + 二次转换多一对
        // 存/取（-0x2c→-0x64）。
        unsigned int ipgNo = (unsigned int)ScanTypeOrInt(isType, str, ok, true);
        // [b28d-solo P223/BM8] ORIG 0x08abf547/0x08abf556 两个独立 nop 岛
        //（+0x1e8/+0x1eb）各自 `jne` 到共享 `mov $0,%ebx` 块 ⇒ 两条独立否定
        // goto + 连续尾标签串；`if(isType)return 0; if(!ok)return 0;` 使 4.4
        // 产就地 `mov $0,%ebx; jmp` 且极性反转。
        if (isType)
            goto daily_is_type;
        if (!ok)
            goto daily_not_ok;
        int fieldA = ScanInt((bool*)0);
        int fieldB = ScanInt((bool*)0);
        // [b28d-solo P292] ORIG 调用点 `call readTime; xor $1,%eax; test %al,%al`
        // ⇒ callee 返回 bool（int 返回产 `test %eax; sete %al`）。mangled 不含
        // 返回类型，改声明零 ABI 风险。
        extern bool readTime(short* buf);
        if (!readTime(t1))
            return -1;
        if (!readTime(t2))
            return -1;
        extern unsigned int uint32Time(short* buf);
        // [b28d-solo P226] ORIG 0x08abf612 `call uint32Time; mov %eax,%edi`（值升格
        // callee-saved 跨第二次 uint32Time 保活，无槽往返）；普通具名局部使 4.4 落槽。
        register int start = (int)uint32Time(t1);
        int end = (int)uint32Time(t2);
        int fieldC = ScanInt((bool*)0);
        // [b28d-solo P167/P123] ORIG 0x08abf5ff 调 `_Rb_tree_iterator::operator==`
        //（eq）非 operator!=；且 `je <printf 岛>` + 正常路径直落 ⇒ 源码为
        // `if (find == end) { 插入 } else { printf; return -1; }`。
        if (m_infoMap.find(ipgNo) == m_infoMap.end()) {
            InfoDaily* info = new InfoDaily(type, ipgNo, (unsigned int)fieldA,
                                            start, end, fieldB, fieldC);
            // [b28d-solo P192/P178] ORIG 无 make_pair 调用，直接 value_type 转换
            // ctor（`_ZNSt4pairIKjPN...EEC1IRjRS3_EEOT_OT0_`）。
            m_infoMap.insert(
                std::map<unsigned int, InfoDaily*>::value_type(ipgNo, info));
        } else {
            // [b28d-solo P185] ORIG rodata 原文（无前缀/无 \n）。
            printf("already registed ipg_no:%u", ipgNo);
            return -1;
        }
    }
daily_is_type:;
daily_not_ok:;
    return 0;
}

void ScriptLoader::ClearScript()
{
}

InfoDaily* ScriptLoader::GetRestrictInfo(unsigned int ipgNo)
{
    std::map<unsigned int, InfoDaily*>::iterator it = m_infoMap.find(ipgNo);
    // [b28c-4 P222] ORIG 0x08abf766：`je 零块(fall-through); body; jmp end` 布局
    // ⇒ 正向包裹 `if (it != end) return it->second;`（否定早返会产反向 je 布局）。
    if (it != m_infoMap.end())
        return it->second;
    return 0;
}

bool ScriptLoader::FindIpgNo(unsigned int ipgNo)
{
    // ORIG 0x8abf70a（Ghidra C）：`return it == end() ? 0 : 1`，即 `find() != end()`
    // （未找到=1）。[b28c-4] ORIG 尾为 operator== + test/je + mov$0/$1 常量岛 ⇒
    // 保留分支的 if 形（直接 `!=` 表达式被 4.4 折成 xor;ret，少 4 条）。
    if (m_infoMap.find(ipgNo) == m_infoMap.end())
        return false;
    return true;
}

// ---- readTime / uint32Time ----
// [b28d-solo P292] 返回类型 bool：ORIG 调用点 `xor $1,%eax; test %al,%al`（bool
// callee 的 `!f()`）；int 返回在 4.4 产 `test %eax; sete %al`。mangled 不含返回类型。
// b28d-solo-CCeraShopAddRestrict: AE（124=124，仅 .o 重定位显示差；P229/N104）。
bool readTime(short* out)
{
    // b28d-45 试形：ORIG 0x08abf16x 先 `movl $0,-0x10`（n）后 `movb $0,-0x11`（ok）
    // ⇒ 交换声明序对齐发射序。
    int n = 0;
    bool ok = false;
    try
    {
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start year";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start month";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start day";
        out[n++] = (short)ScanInt(&ok);
        if (!ok)
            throw "start time";
        // [b28d-solo P229/N104] ORIG 0x08abf2c3：正常路径尾在 `call __cxa_throw`
        // 之后立即 `mov $0x1,%ebx; jmp <函数尾>`（try 正常出口），4.4 把该物化
        // 落在函数尾共享块 ⇒ 源码须把 `return 1` 写在 try 块内。
        return true;
    }
    catch (const char* msg)
    {
        LogManager::logFormat(1, "../RDARScriptCeraShopAddRestrictType.cpp", "readTime",
                              37, "readTime() Load fail. %s", msg);
        return false;
    }
}

unsigned int uint32Time(short* buf)
{
    time_t t = time(0);
    struct tm* tm = localtime(&t);
    tm->tm_year = buf[0] - 1900;
    tm->tm_mon = buf[1] - 1;
    tm->tm_mday = buf[2];
    tm->tm_hour = buf[3];
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return (unsigned int)mktime(tm);
}

// ---- 命名空间级辅助（ORIG 报告 CerashopAddRestrict/*.md）----

// ORIG 0x080e0a43（static，L mangled；FindUserInfoDetail.md 全文转写）：
//   idxMap.find(key)：未命中 insert(key,0) 取返回迭代器；it->second += add；
//   dailyMap.find(key)：命中 → now<nextDate 时 (count+idx)<=field8 → 1 否则 0；
//     超期 → count=0，idx<=field8 → 1 否则 0；未命中 → field8<idx → 0 否则 1。
// b28d-solo-CCeraShopAddRestrict: NEAR→N 封顶（129=129、p167 CLEAR；残余 N95 槽基差 8
// + N86 insert sret 槽位；P196 比较方向与 m_count unsigned 已修）。
static int FindUserInfoDetail(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, paramDaily*>& dailyMap,
    unsigned int key, InfoDaily* info, unsigned int add)
{
    std::map<unsigned int, unsigned int>::iterator it = idxMap.find(key);
    if (it == idxMap.end()) {
        it = idxMap.insert(
                 std::map<unsigned int, unsigned int>::value_type(key, 0)).first;
    }
    it->second += add;

    std::map<unsigned int, paramDaily*>::iterator it2 = dailyMap.find(key);
    if (it2 != dailyMap.end()) {
        paramDaily* daily = it2->second;
        time_t now = time(0);
        if (now < daily->m_nextDate) {
            if ((unsigned int)(daily->m_count + it->second) <=
                (unsigned int)info->m_field8)
                return 1;
        } else {
            daily->m_count = 0;
            if ((unsigned int)it->second <= (unsigned int)info->m_field8)
                return 1;
        }
        return 0;
    }
    // [b28d-solo P196] ORIG 0x080e0bcd 段：先载 it->second 到 %edx、后载
    // info->m_field8 到 %eax，`cmp %eax,%edx; seta` ⇒ 源码须写
    // `it->second > m_field8`；反向书写 `m_field8 < it->second` 使 4.4
    // 先物化左操作数到 %ebx 并产 `setb`（比较方向反演）。
    if ((unsigned int)it->second > (unsigned int)info->m_field8)
        return 0;
    return 1;
}

// ORIG 0x080e10d7（static，L mangled；GetCalcDay.md 全文转写）：
//   days=(m_end-m_start)/0x15180+1；m_limit>0 → days=m_limit；t=m_start，
//   循环 t+=days*0x15180 至 t>now；localtime_r 置 tm_hour=G_CEnvironment()+0x37c、
//   min/sec=0，mktime 返回。
static int GetCalcDay(const InfoDaily* info)
{
    int days = (info->m_end - info->m_start) / 0x15180 + 1;
    if (info->m_limit > 0)
        days = info->m_limit;
    time_t t = (time_t)info->m_start;
    time_t now = time(0);
    while (t <= now)
        t += days * 0x15180;
    struct tm local44;
    localtime_r(&t, &local44);
    local44.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    local44.tm_min = 0;
    local44.tm_sec = 0;
    return (int)mktime(&local44);
}

// ORIG 0x080e1196（命名空间级，T mangled；InputBuyInfoDetail.md 全文转写）：
//   m.find(key)：命中 → m_count += count；未命中 → new paramDaily 清零、
//   m_count=count、m_ipgNo=key 后 insert；尾 m_nextDate=GetCalcDay(info)，返回 pd。
// b28d-solo-CCeraShopAddRestrict: AE（77=77，仅 .o 重定位显示差；P211档③ m_count
// unsigned + P278 pair 临时 + P90/P299 value-init）。
paramDaily* InputBuyInfoDetail(
    std::map<unsigned int, paramDaily*>& m,
    const InfoDaily* info, unsigned int key, unsigned int count)
{
    paramDaily* result = 0;
    std::map<unsigned int, paramDaily*>::iterator it = m.find(key);
    if (it != m.end()) {
        result = it->second;
        // [b28c-4 P41/P211] ORIG 0x080e11fb 段 = `mov 0x4(%eax),%eax; mov %eax,%edx;
        // add 0x14(%ebp),%edx`（内存直加）；`(int)count` 反使 4.4 先载 eax。
        result->m_count += count;
    } else {
        // [b28d-solo P90/P299] ORIG 0x080e120e：`call _Znwj` 后立即内联 3 条
        // `movl $0x0,(|0x4|0x8)(%eax)` 且 pd 存槽在清零之后 ⇒ 原码是 value-init
        // `new paramDaily()`（POD 零初始化），不是「具名 pd + 三条手动清零」
        //（后者 4.4 先存槽再逐条重载，多 3 条 mov）。
        paramDaily* pd = new paramDaily();
        pd->m_count = count;
        pd->m_ipgNo = key;
        // [b28d-solo P278] ORIG 0x080e1248：pair 临时直构于 -0x14（ctor this 即
        // `lea -0x14,(%esp)`）、insert sret -0x1c；具名 p 形把 pair 落 -0x28 且
        // insert sret 落 -0x14（槽序整体反向）。类型仍用 map::value_type 直构，
        // 保持 ORIG 的 pair<unsigned&,paramDaily*&> 转换 ctor 符号。
        m.insert(std::map<unsigned int, paramDaily*>::value_type(key, pd));
        result = pd;
    }
    result->m_nextDate = GetCalcDay(info);
    return result;
}

Manager::~Manager()
{
}

bool Manager::Load()
{
    // ORIG 0x080e0e08：printf("[!] Loading CeraShop Add Restrict Script ...") 后
    // 调 ScriptLoader::LoadScript()；非 0 返回 0，否则 1。
    printf("[!] Loading CeraShop Add Restrict Type script - ");  // ORIG 串无 \n，仅尾随空格（带 \n 会被 GCC 转 puts）
    // [b28c-4 P0] 存量 `register unsigned char nz asm("al")` 已清除（全域禁绑，
    // N96 登记），按 P168a① 复测自然形：ORIG 尾 test/setne/test/je 常量岛。
    if (m_loader.LoadScript() != 0)
        return 0;
    return 1;
}

int Manager::Destroy()
{
    // ORIG 0x080e0e40：返回 1；经 ScriptInterface 虚槽调用时返回值被忽略，
    // 槽签名 void（Arad_DataManager.h ScriptInterface）。
    return 1;
}

bool Manager::FindIpgNo(unsigned int ipgNo)
{
    return m_loader.FindIpgNo(ipgNo);
}

// ORIG 0x080e0bee（报告 CheckBuyableProduct.md 全文转写）：
//   type==0 → 清 m_field30/m_field48；GetRestrictInfo 未命中 → 1；
//   info->m_end < time(0) → 0；m_type==0/1 → FindUserInfoDetail(...)；否则 0。
int Manager::CheckBuyableProduct(CUser* user, unsigned int ipgNo,
                                 unsigned int itemIdx, int type)
{
    UserInfo& ui = user->m_ceraUserInfo;
    if (type == 0) {
        ui.m_field30.clear();
        ui.m_field48.clear();
    }
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return 1;
    if (info->m_end < (int)time(0))
        return 0;
    // [b28c-4 P46/P208] ORIG 0x080e0bee：m_type 单次装载 + `test;je / cmp $1;je /
    // jmp 零块` 决策链，body 全部后置 ⇒ switch 形（if 链被 4.4 就地展开成反向
    // `jne skip` 且二次装载 m_type）。
    switch (info->m_type) {
    case 0:
        return FindUserInfoDetail(ui.m_field30, ui.m_accountDaily,
                                  ipgNo, info, itemIdx);
    case 1:
        return FindUserInfoDetail(ui.m_field48, ui.m_characDaily,
                                  ipgNo, info, itemIdx);
    default:
        return 0;
    }
}

// ORIG T 0x080e0d14（LinkE2.JpnExImpl 批次修正）：static 成员；定义体内经
// ARAD::Singleton<Manager>::Get() 调 FindIpgNo/InputBuyInfo/SendSyncPacket。
void Manager::UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                        unsigned int itemIdx)
{
    if (!ARAD::Singleton<Manager>::Get()->FindIpgNo(ipgNo))
        return;
    DBInputData data;
    // [b28c-4 P0] 存量 `register unsigned char ok asm("eax")` 已清除（全域禁绑，
    // N96 登记）；ORIG 调用点是 int callee 返回值的 al 直测（P3），改表达式直读
    // 复测。data 槽基：ORIG -0x10（sizeof 已实证 0x8，见 CUser.h b28c 代落注）。
    if (ARAD::Singleton<Manager>::Get()->InputBuyInfo(user, ipgNo, itemIdx, data)) {
        // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e17ba）。
        DB_SaveCerashopAddRestrictType::makeRequest(user, data);
        // ORIG 直接调 static SendSyncPacket（无 this、无 Get()；注释 0x817915c 实证）
        SendSyncPacket(user);
    }
}

void Manager::ClearBuyRestrictItem(CUser* user)
{
    // [b28c-4 P183] ORIG 0x080e0da4 每处从 user 重算绝对偏移（add $0x8d1e4/...），
    // 无引用缓存槽（引用形多 -0xc 槽 + 帧 0x28→0x18 差）。
    user->m_ceraUserInfo.m_accountDaily.clear();
    user->m_ceraUserInfo.m_characDaily.clear();
    user->m_ceraUserInfo.m_field30.clear();
    user->m_ceraUserInfo.m_field48.clear();
}

void Manager::LoadRequestBuyRestrictItem(CUser* user)
{
    // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e1ff6）。
    DB_LoadCerashopAddRestrictType::makeRequest(user);
}

void Manager::SendSyncPacket(CUser* user)
{
    // ORIG：ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user)
    // 符号 _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）
    ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user);
}

// ORIG 0x080e0e68（报告 GetItemListForPacket.md 全文转写）：
//   遍历 m（begin/end 各求值一次，while 循环后缀 ++）：
//   GetRestrictInfo(it->first) 命中且 info->m_field8 <= it->second->m_count
//   （无符号比较 setbe）时 out.push_back(it->first)。
void Manager::GetItemListForPacket(
    std::vector<unsigned int, std::allocator<unsigned int> >& out,
    std::map<unsigned int, paramDaily*,
             std::less<unsigned int>,
             std::allocator<std::pair<unsigned int const, paramDaily*> > >& m)
{
    std::map<unsigned int, paramDaily*>::iterator it = m.begin();
    std::map<unsigned int, paramDaily*>::iterator end = m.end();
    while (it != end) {
        InfoDaily* info = m_loader.GetRestrictInfo(it->first);
        if (info != 0) {
            if ((unsigned int)info->m_field8 <= (unsigned int)it->second->m_count)
                out.push_back(it->first);
        }
        it++;
    }
}

// ORIG 0x080e0f3a（报告 makeSyncPacket.md 全文转写）：
//   m_accountDaily.size()+m_characDaily.size() 非零时，vector 收集两表
//   GetItemListForPacket 结果；非空则 PacketGuard 组 0x222 头：
//   put_int(size) + 逐项 put_int，finalize(true) 后 CUser::Send。
// b28d-solo-CCeraShopAddRestrict: AE（123=123，仅 .o 重定位显示差；P274 + P183 +
// P292 + P223）。
void Manager::makeSyncPacket(CUser* user)
{
    UserInfo& info = user->m_ceraUserInfo;
    // [b28d-solo P274/P211] ORIG 0x080e0f69 累加段 `mov -0x10(%ebp),%edx;
    // add %edx,%eax; mov %eax,-0x10(%ebp)`（size2 在 %eax 为左操作数）⇒ 源写
    // `total = size2 + total;`；`total += size2;` 使 4.4 折 `add %eax,-0x14`
    // 内存直加（少 2 条 + 槽号漂移）。
    int total = (int)info.m_accountDaily.size();
    total = (int)info.m_characDaily.size() + (unsigned int)total;
    // [b28d-solo P223] ORIG 0x080e10cf 尾空标签 nop 岛（`je +0x195` 落点即
    // `nop`，紧接 epilogue）⇒ 负条件 goto 共享尾空标签复现。
    if (total == 0)
        goto sync_packet_end;
    {
        std::vector<unsigned int> list;
        GetItemListForPacket(list, info.m_accountDaily);
        GetItemListForPacket(list, info.m_characDaily);
        // [b28d-solo P292/P168a] ORIG 0x080e0fc1 `call empty; test %al,%al; jne
        // <vector 析构前>` = 正极性直接判 empty；`if (!list.empty())` 使 4.4 多
        // 一条 `xor $0x1,%eax`（bool 取反物化）⇒ 正极性空 then + else 形。
        if (list.empty()) {
        } else {
            PacketGuard packet;
            // [b28d-solo P183/P89] ORIG 每处 `lea -0x2c(%ebp),%eax; mov %eax,(%esp)`
            // 重算 packet 地址；具名 `InterfacePacketBuf* buf` 多一个 -0x10 槽并
            // 使 vector 槽整体深移 4。
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x222);
            ((InterfacePacketBuf*)&packet)->put_int((int)list.size());
            for (unsigned int i = 0; i < list.size(); ++i)
                ((InterfacePacketBuf*)&packet)->put_int((int)list[i]);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            user->Send(packet);
        }
    }
sync_packet_end:;
}

// ORIG 0x080e129a（报告 InputBuyInfo.md 全文转写）：
//   GetRestrictInfo 未命中 → 0；out.m_pInfo=info；m_type==0/1 时先在
//   m_field30/m_field48 中 find(ipgNo) 命中则 erase，再
//   InputBuyInfoDetail(...) 并把返回值写入 out.m_result；
//   尾判 out.m_result==0 → 0，否则 1；其它 m_type → 0。
// [b28c-4 档①] 返回类型 int→bool（Main 代落声明 CUser.h:1522）：ORIG 0x080e129a
// 出口仅 `mov $0x0/$0x1,%eax`，且唯一调用点 UpdateBuyableRestrictItem 0x080e0d14 为
// `call; test %al,%al; je` 单字节直判——int callee 在 4.4 恒多 setne 归一段
//（探针 /tmp/l4/al.cpp m1-m4 四形全败），唯 bool callee 复现。
// b28d-solo-CCeraShopAddRestrict: AE（121=121，仅 .o 重定位显示差；P211档③
// m_result 具型化 paramDaily* + P251/P261 switch）。
bool Manager::InputBuyInfo(CUser* user, unsigned int ipgNo, unsigned int itemIdx,
                           DBInputData& out)
{
    InfoDaily* info = m_loader.GetRestrictInfo(ipgNo);
    if (info == 0)
        return false;
    out.m_pInfo = info;
    UserInfo& ui = user->m_ceraUserInfo;
    // b28d-45 P251/P261：ORIG 0x080e12d0 为无槽 cmp 链
    // `test %eax,%eax; je case0; cmp $0x1,%eax; je case1; jmp default` ⇒ switch。
    switch (info->m_type) {
    case 0: {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field30.find(ipgNo);
        if (it != ui.m_field30.end())
            ui.m_field30.erase(it);
        // [b28d-solo P211档③] m_result 具型化为 paramDaily*（与 callee 返回同型）⇒
        // ORIG 0x080e1375 `mov 0x18(%ebp),%edx; mov %eax,(%edx)` LHS-first 直存；
        // 原 int 字段异型恒产 `mov %eax,%edx; mov 0x18(%ebp),%eax; mov %edx,(%eax)`。
        out.m_result = InputBuyInfoDetail(ui.m_accountDaily, info,
                                          ipgNo, itemIdx);
        break;
    }
    case 1: {
        std::map<unsigned int, unsigned int>::iterator it = ui.m_field48.find(ipgNo);
        if (it != ui.m_field48.end())
            ui.m_field48.erase(it);
        out.m_result = InputBuyInfoDetail(ui.m_characDaily, info,
                                          ipgNo, itemIdx);
        break;
    }
    default:
        return false;
    }
    if (out.m_result == 0)
        return false;
    return true;
}

// IsBuyableRestrictItem —— ORIG 0x080e0ce2 (T)
// 静态转发：ARAD::Singleton<Manager>::Get()->CheckBuyableProduct(user, ipgNo, itemIdx, type)。
// 返回 CheckBuyableProduct 的结果（1=可购买）；调用方（ItemVendingMachine::
// ProcessBuyRequestEx）取反判定「不可购买」。
bool Manager::IsBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                   unsigned int itemIdx, int type)
{
    return ARAD::Singleton<CerashopAddRestrict::Manager>::Get()
        ->CheckBuyableProduct(user, ipgNo, itemIdx, type);
}

// ORIG 0x080e142a sz 0xcb（_ZN19CerashopAddRestrict7Manager11LoadProcessE
// RSt3mapIjjSt4lessIjESaISt4pairIKjjEEERS1_IjPNS_10paramDailyES3_SaIS4_IS5_SB_EEEP35SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE；
// objdump 转写，无独立报告）：idxMap.clear()；循环 i=0..sig+0x00 的 count
// （unsigned 比较）：new paramDaily（+0x00/+0x04/+0x08 清零），从 sig+0x08
// 起以 0xc 步长取第 i 元素的 3 个 uint 依序填入
// pd->{m_ipgNo,m_count,m_nextDate}，再 dailyMap.insert(pair(elem[0], pd))。
// idxMap 仅被 clear（ORIG 无其它访问）。
// b28d-solo-CCeraShopAddRestrict: AE（61=61，仅 .o 重定位显示差；P90/P299
// `new paramDaily()` + P291 SIG m_items 视图）。
void CerashopAddRestrict::Manager::LoadProcess(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, CerashopAddRestrict::paramDaily*>& dailyMap,
    SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig)
{
    idxMap.clear();
    // [b28d-solo P291/P214①] 元素地址必须走 SIG 具名数组成员视图
    //（SigTypes_B.h `m_items[1]`）：`&sig->m_items[i]` 让 4.4 产 ORIG 的
    // `... shl $0x2; add sig; add $8`（f16 探针实测）；`(char*)sig + i*0xc + 8`
    // 恒折 `add $8; lea (sig,%eax,1)`（多 1 条 + 寄存器序反）。
    for (unsigned int i = 0; sig->m_count > i; ++i)
    {
        // [b28d-solo P90/P299] 同 InputBuyInfoDetail：ORIG 0x080e1447 的
        // `call _Znwj` 后立即内联 3 条清零、pd 存槽在后 ⇒ value-init 形。
        paramDaily* pd = new paramDaily();
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE::Elem* elem = &sig->m_items[i];
        pd->m_ipgNo = elem->m_ipgNo;
        pd->m_count = elem->m_count;
        pd->m_nextDate = elem->m_nextDate;
        // [b28c-4] ORIG pair 首参照 `mov -0x24(%ebp),%eax`（=&pd->m_ipgNo）⇒
        // 实参是 pd->m_ipgNo 左值（值与 elem[0] 同、地址不同）。
        dailyMap.insert(
            std::map<unsigned int, paramDaily*>::value_type(pd->m_ipgNo, pd));
    }
}

}  // namespace CerashopAddRestrict
// ============================================================================
// CCeraShopBonusItemEvent / CEventCeraShopRewardPoint（商城事件）
// ============================================================================
// 类声明见 CCeraShopBonusItemEvent.h（权威，继承 CGuildServerProxy.h 的
// CEventBase；vtable 序 / +0x0c m_rate 布局依 ORIG 报告）。方法定义如下。

// ORIG C1 0x0810a2dc：仅 CEventBase::CEventBase() + 本类 vptr，无成员初始化
// （InitEventManager new 0x10；m_rate 由 StartEvent(Word_Param) 延迟写入）。
CCeraShopBonusItemEvent::CCeraShopBonusItemEvent()
{
}

CCeraShopBonusItemEvent::~CCeraShopBonusItemEvent()
{
}

void CCeraShopBonusItemEvent::StartEvent()
{
}

// ORIG StartEvent(Word_Param) 0x0810a34c：SetEventFlag(true)；参数低 ushort
// （m_duration）为 0 或 >1000 → 日志且不写 m_rate；否则写 +0x0c 并记日志。
void CCeraShopBonusItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    if (param.m_duration == 0 || 1000 < param.m_duration) {
        // [b28c-30] P227 全表达式临时链：ORIG 0x0810a361 段 vararg
        // `(unsigned int)param.m_duration` 在 ctor 前求值并入 %ebx 跨 ctor 保活
        // （push %ebx/sub $0x34 三件套）；具名 tr 两语句使 4.4 ctor 后重取。
        cMyTrace(__FUNCTION__, 0x1a, 0)
            ("Can not start Cera shop bonus item event. Bonus rate is too big(%d)",
             (unsigned int)param.m_duration);
    } else {
        m_rate = (unsigned int)param.m_duration;
        cMyTrace(__FUNCTION__, 0x20, 0)
            ("Cera shop bonus item event Start");
    }
}

void CCeraShopBonusItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x26, 0);
    tr("Cera shop bonus item event End");
}
// getBonusRate（ORIG W 0x0817a2e2）：直返 +0x0c bonusRate（mov 0xc(%eax),%eax;
// ret；P9 直返形态，StartEvent(unsigned short) 以 4 字节写入同一字段）。
int CCeraShopBonusItemEvent::getBonusRate() const
{
    return (int)m_rate;
}
