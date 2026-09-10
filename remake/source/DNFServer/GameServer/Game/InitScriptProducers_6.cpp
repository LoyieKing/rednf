// ============================================================================
// InitScriptProducers_6 —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数·第 6 批。
//
// 本 TU 独家提供下列 ORIG T 符号的真实定义（逐指令转写自 df_game_r，
// 各函数 ORIG 地址见实现处注释；节名键串取自 .rodata 实测）：
//   importAvatarRouletteList            0x089116eb (+ Detail 0x089115b1)
//   AvatarRoulette::{C1,D1,clear}       0x08911b98 / 0x0837e484 / 0x08911cf6
//   importBloodDungeonScript            0x08909016
//   importCharacLinkSystemScript        0x08909529
//   importCharacterCreateCreatureGiftScript 0x0896001a
//   importCharacterCreateGiftScript     0x0895eda1
//   importConditionEventChkDungeonScript 0x08913891
//   importConditionEventScript          0x0890e868
//   importDBLogItem                     0x0890dbba
//   importDailyScheduleEtcScript        0x0891826c
//   importDeathTowerScript              0x089089f3
//   importDimensionActivationData       0x089160f0
//   importEmotionTypeTagsScript         0x0890de85
//   importEquipmentGradeSampleInfo      0x0890e18d
// 及其共享叶子依赖（ORIG 均为 T，源码此前无任何定义，已广播认领）：
//   ConvertStringToTime                 0x088c2341
//   ConvertHHMMSSStringToInt            0x088c25df
//   ConvertWeekdayStringToInt           0x088c284a
//   getCharacterJob                     0x088be57e
//   getGrowTypeIndex                    0x088be63d
//   makeFairPVPEquipmentPackageKey      0x088945ad
//   tokenizeStr                         0x088945e2
//   getCharacterJobToEnumValue          0x089bbc0f
//
// 统一模式：loadRDARScriptFile("", path) → ScanType 分派节名 → ScanInt/
// ScanFloat/ScanStr 字段回填。ORIG 中 string→分支 的静态 map<const char*,int>
// （__lookup/__init_lock/__inited BSS 桩）是编译器对「按行号分派」的缓存 lowering，
// 观测语义等价于顺序串比较——重建一律采用 if/else 链，不引入全局可变状态。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>
#include <map>

#include "CDataManager.h"             // 脚本类型权威布局（stDeathTower_t 等）
#include "sync_script.h"             // STEquipmentPartSetPiece/STEquipmentScript
                                     // （DataManagerInitDepends.h 前置要求）
#include "DataManagerInitDepends.h"   // 本批 import* 声明 + AvatarRoulette 完整视图
#include "DNFLexWrapper.h"            // loadRDARScriptFile/unload/Scan*/ScanTypeOrInt
#include "CEnvironment.h"             // less_TCHAR / SpinLock（getGrowTypeIndex __lookup）

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

// ============================================================================
// 共享叶子 helper
// ============================================================================

// ----------------------------------------------------------------------------
// ConvertStringToTime —— ORIG T 0x088c2341。"YYYY-MM-DD HH:MM:SS"（19 字符）
// → mktime。substr(0,4)/(5,2)/(8,2)/(11,2)/(14,2)/(17,2) 各 atoi 后直接写入
// 栈上 struct tm 对应槽位（tm_sec..tm_year；wday/yday/isdst 不初始化，ORIG 如此，
// mktime 会归一化）。长度非 19 返回 false；成功写 out 并返回 true。
// ----------------------------------------------------------------------------
bool ConvertStringToTime(std::string s, long& out)
{
    if (s.size() != 0x13)
    {
        return false;
    }
    struct tm t;
    t.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;  // -0x38（ORIG 首段 sub $0x76c）
    t.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;   // -0x3c
    t.tm_mday = atoi(s.substr(8, 2).c_str());   // -0x40
    t.tm_hour = atoi(s.substr(11, 2).c_str());  // -0x44
    t.tm_min = atoi(s.substr(14, 2).c_str());   // -0x48
    t.tm_sec = atoi(s.substr(17, 2).c_str());   // -0x4c
    // tm_wday/tm_yday/tm_isdst 保持未初始化（与 ORIG 栈槽一致，mktime 归一化）
    out = mktime(&t);
    return true;
}

// ----------------------------------------------------------------------------
// ConvertHHMMSSStringToInt —— ORIG T 0x088c25df。"HH:MM:SS"（8 字符）：
// h=atoi(substr(0,2))、m=atoi(substr(3,2))、s=atoi(substr(6,2))。
// 长度非 8 返回 false。
// ----------------------------------------------------------------------------
bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h,
                              unsigned char& m, unsigned char& sec)
{
    if (s.size() != 8)
    {
        return false;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    sec = static_cast<unsigned char>(atoi(s.substr(6, 2).c_str()));
    return true;
}

// ----------------------------------------------------------------------------
// ConvertWeekdayStringToInt —— ORIG T 0x088c284a。SUN..SAT→0..6、ALL→7；
// 未命中返回 false 且不触碰 out（ORIG 仅在命中路径写 (%edx)）。
// ----------------------------------------------------------------------------
// b28d-solo-InitScriptProducers_6: P63 共享尾（else-if 链 + 尾部统一 return true）。
// ORIG 用 `s.compare("X") == 0`（int compare → `test %eax,%eax; sete %al; test; je`），
// 非 operator==(bool)。ORIG 0x088c284a。
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out)
{
    if (s.compare("SUN") == 0) out = 0;
    else if (s.compare("MON") == 0) out = 1;
    else if (s.compare("TUE") == 0) out = 2;
    else if (s.compare("WED") == 0) out = 3;
    else if (s.compare("THU") == 0) out = 4;
    else if (s.compare("FRI") == 0) out = 5;
    else if (s.compare("SAT") == 0) out = 6;
    else if (s.compare("ALL") == 0) out = 7;
    else return false;
    return true;
}

// ----------------------------------------------------------------------------
// tokenizeStr —— ORIG T 0x088945e2。先求首/尾锚点并 clear 输出容器，随后
// while(end!=npos || start!=npos) 循环 push_back(substr(start,end-start)) 并
// 推进双锚点。循环条件为「或」（88946cd 处 jne/je 组合实证），与常规实现不同。
// ----------------------------------------------------------------------------
void tokenizeStr(const std::string& s, std::vector<std::string>& out,
                 const std::string& delims)
{
    std::size_t start = s.find_first_not_of(delims, 0);
    std::size_t end = s.find_first_of(delims, start);
    out.clear();
    while (end != std::string::npos || start != std::string::npos)
    {
        out.push_back(s.substr(start, end - start));
        start = s.find_first_not_of(delims, end);
        end = s.find_first_of(delims, start);
    }
}

// ----------------------------------------------------------------------------
// getCharacterJob —— ORIG T 0x088be57e（定义在 SyncScriptDeps.cpp，本 TU 仅声明
// 引用）。全局作业名表 g_characterJobNames（ORIG .data 0x93f4d40，12 项实测）：
// "[swordman]".."[creator mage]" 共 11 项 + "[all]"；i=0..10 命中返回 i，否则 11。
// ----------------------------------------------------------------------------
int getCharacterJob(const char* name);

// ----------------------------------------------------------------------------
// makeFairPVPEquipmentPackageKey —— ORIG T 0x088945ad（定义在
// InitScriptProducers_2.cpp，归属协商结果，本 TU 仅声明引用）：
// 两参各截 short 入 4 字节槽后整槽返回 ((short)b<<16)|(short)a。
// ----------------------------------------------------------------------------
int makeFairPVPEquipmentPackageKey(int a, int b);

// ----------------------------------------------------------------------------
// getCharacterJobToEnumValue —— ORIG T 0x089bbc0f（定义在 CDataManager.cpp，
// 本 TU 仅声明引用）。std::string== 链：swordman..creator mage→0..10、
// common→11、其余 -1。
// ----------------------------------------------------------------------------
int getCharacterJobToEnumValue(std::string& s);

// ----------------------------------------------------------------------------
// getGrowTypeIndex —— ORIG T 0x088be63d（0xc7b 字节，定义归本 TU）。
// ORIG 结构（/tmp/getGrowTypeIndex.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x8b5 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 25 条，表尾释放锁、置 __inited 并跳回 guard(088be650) 重新 find 派发；
// 建表后 find 命中按码组 return："none"→0；weaponmaster/nenmaster/ranger/
// elementalmaster/crusader/rogue/elementalbomber(0x8ba..0x8bf,0x8c1)→1；
// soulbringer/striker/launcher/summoner/infighter/necromancer/glacialmaster
// (0x8c7..0x8cc,0x8ce)→2；berserker/streetfighter/mechanic/battlemage/
// exorcist(0x8d4..0x8d8)→3；asura/grappler/spitfire/witch/avenger
// (0x8dd..0x8e1)→4；未命中/未知码 return 0xffff（local_24 入口初值）。
// 键比较为 less_TCHAR（大小写无关），与旧 strcmp 线性链语义不同。
// ----------------------------------------------------------------------------
typedef std::map<const char*, int, less_TCHAR> GrowTypeMap;

// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — p167 调用点差集 CLEAR（71=71，
// 语义完整）；残差 N82（帧/槽号 673 vs 687）+ N89（libstdc++ 模板全内联）。
// [SweepInitProd9 本轮] 负极性反例实测（勿再试）：把 25 个 case 体统一改为
// `if (!__inited) { insert } else { return N; }`（P333 负极性形，与同 TU
// importDimensionActivationData 的 break 版不同——此处 else 是 return）后
// ourN 687→**737**（gap −14→−64）、jcc 目标块差异段 13→13 且 jcc 数 91/109→
// 91/127，明显反劣 ⇒ 已回滚。P333 的负极性守卫形仅在 else 为 `break` 时成立。
// [SweepInitProd9 本轮·回边方向审计] ORIG backward 2 / 我方 1：两条 continue
// （0x7b2 guard 重试 / 0x1343 尾部置位）汇聚共享尾块 0x134c → `jmp 687`(+0x13
// 循环头)，ORIG 为两条独立 jmp 直达 +0x13 ⇒ 共享块合并，**回边未丢失**。
// ORIG 0x088be63d。
int getGrowTypeIndex(const char* name)
{
    int idx = -1;                     // ORIG 首指令 movl $0xffff,-0x20
    for (;;)
    {
        static GrowTypeMap __lookup;  // ORIG ...E8__lookup
        static SpinLock __init_lock;                             // ORIG ...E11__init_lock
        static bool __inited = false;                            // ORIG ...E8__inited
        // [L16 本轮] 极性翻正（P222/P214①）：ORIG 为 `movzbl __inited; test %al,%al;
        // je else` 正极性（无 xor），我方原 `!__inited` 形多出发 `xor $0x1,%eax`。
        if (__inited)
        {
            GrowTypeMap::const_iterator it =
                __lookup.find(name);
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;               // ORIG 088be81f jmp 088be650（回 guard 重检）
            }
            idx = 0;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式（__inited）组内各码经 je 链汇聚共享结果槽后 return。
        case 0:
        case 0x8b5:
            if (__inited) return 0;
            __lookup.insert(GrowTypeMap::value_type("none", 0x8b5));
            /* fall through */
        case 0x8ba:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("weaponmaster", 0x8ba));
            /* fall through */
        case 0x8bb:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("nenmaster", 0x8bb));
            /* fall through */
        case 0x8bc:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("ranger", 0x8bc));
            /* fall through */
        case 0x8bd:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalmaster", 0x8bd));
            /* fall through */
        case 0x8be:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("crusader", 0x8be));
            /* fall through */
        case 0x8bf:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("rogue", 0x8bf));
            /* fall through */
        case 0x8c1:
            if (__inited) return 1;
            __lookup.insert(GrowTypeMap::value_type("elementalbomber", 0x8c1));
            /* fall through */
        case 0x8c7:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("soulbringer", 0x8c7));
            /* fall through */
        case 0x8c8:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("striker", 0x8c8));
            /* fall through */
        case 0x8c9:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("launcher", 0x8c9));
            /* fall through */
        case 0x8ca:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("summoner", 0x8ca));
            /* fall through */
        case 0x8cb:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("infighter", 0x8cb));
            /* fall through */
        case 0x8cc:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("necromancer", 0x8cc));
            /* fall through */
        case 0x8ce:
            if (__inited) return 2;
            __lookup.insert(GrowTypeMap::value_type("glacialmaster", 0x8ce));
            /* fall through */
        case 0x8d4:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("berserker", 0x8d4));
            /* fall through */
        case 0x8d5:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("streetfighter", 0x8d5));
            /* fall through */
        case 0x8d6:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("mechanic", 0x8d6));
            /* fall through */
        case 0x8d7:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("battlemage", 0x8d7));
            /* fall through */
        case 0x8d8:
            if (__inited) return 3;
            __lookup.insert(GrowTypeMap::value_type("exorcist", 0x8d8));
            /* fall through */
        case 0x8dd:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("asura", 0x8dd));
            /* fall through */
        case 0x8de:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("grappler", 0x8de));
            /* fall through */
        case 0x8df:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("spitfire", 0x8df));
            /* fall through */
        case 0x8e0:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("witch", 0x8e0));
            /* fall through */
        case 0x8e1:
            if (__inited) return 4;
            __lookup.insert(GrowTypeMap::value_type("avenger", 0x8e1));
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;                   // ORIG 088bf2a4 jmp 088be650 重派发
        }
        return 0xffff;                  // ORIG 未命中/未知码：local_24 初值
    }
}

// ============================================================================
// AvatarRoulette 生命周期（ORIG C1 0x08911b98 / D1 0x0837e484 / clear 0x8911cf6）
// ============================================================================

AvatarRoulette::AvatarRoulette()
{
    // 两数组（m_avatars[11]/m_stikers[11]）由成员默认构造完成（ORIG 循环
    // 调 vector C1Ev 等价），随后调 clear。
    clear();
}

AvatarRoulette::~AvatarRoulette()
{
    // ORIG D1：逆序析构 m_stikers[10]..[0]、再 m_avatars[10]..[0]
    // （编译器生成的数组析构等价于此）。
}

// clear —— ORIG 0x8911cf6：j=0..10 双层清空 avatar 数组；stiker 数组不动。
// b28d-21: IDENTICAL（55=55）— P183 删 `vector& bucket = m_avatars[j]` 引用缓存
// （ORIG 每轮重发 j*12+0x14 全链）+ 具名 int n 槽 + 有符号比较（setl）。
void AvatarRoulette::clear()
{
    // b28d-21：ORIG 0x08911cf6 每轮都重发 `j*12+0x14` 完整寻址（P183 禁
    // `vector& bucket = m_avatars[j]` 缓存引用），且 size() 结果落具名 int 槽
    //（ORIG -0xc 每 j 轮算一次；循环尾 `cmp -0xc(%ebp),%eax; setl` 有符号比较）。
    for (int j = 0; j <= 10; ++j)
    {
        // ORIG 槽序：k=-0x10（深）、n=-0xc（浅）⇒ k 先声明/先使用。
        int k = 0;
        int n = static_cast<int>(m_avatars[j].size());
        for (; k < n; ++k)
        {
            m_avatars[j][k].clear();
        }
        m_avatars[j].clear();
    }
}

// ============================================================================
// importAvatarRouletteListDetail —— ORIG T 0x089115b1
//   bool (AvatarRoulette&, std::string&, ENUM_CHARACTERJOB)
// 内层装载器：至多 9 次 ScanTypeOrInt 取数值（遇 TYPE 提前终止），累计入局部
// pairs{value,3}；pairs 非空时整体 push 进 m_avatars[job]，再 ScanInt 一枚
// sticker push 进 m_stikers[job]。恒返回 true。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（97=97）— ORIG 0x089115b1。value(-0x28，预清 0
// 且 ScanInt 结果复用)、isType(-0x29)/isInt(-0x2a)；pair 直构
// `pair<unsigned long,int>(value, 3)`——第二实参须为**右值字面量**（p167 实证
// ORIG 为 `C1IRiiEEOT_OT0_` = pair<int&,int&&>，具名 three 左值会选错重载）；job 形参直用。
bool importAvatarRouletteListDetail(AvatarRoulette& out,
                                    std::string& token,
                                    ENUM_CHARACTERJOB job)
{
    std::vector<std::pair<unsigned long, int> > pairs;
    int value = 0;                      // ORIG -0x28
    bool isType = false;
    bool isInt = false;

    for (int i = 0; i <= 8; ++i)
    {
        value = ScanTypeOrInt(isType, token, isInt, true);
        if (isType)
        {
            break;   // TYPE token 已留在 token 中供上层判节尾
        }
        // ORIG：第二实参是右值字面量（mangled `C1IRiiEEOT_OT0_`，
        // p167 实证 pair<int&,int&&>），编译器为其分配临时槽 -0x10。
        pairs.push_back(std::pair<unsigned long, int>(value, 3));
    }

    if (!pairs.empty())
    {
        out.m_avatars[job].push_back(pairs);   // &out[job*0xc+0x14]（+0x14 数组基址实证）
        value = ScanInt((bool*)0);
        out.m_stikers[job].push_back(value);   // &out[job*0xc+0x98]
    }
    return true;
}

// ============================================================================
// importAvatarRouletteList —— ORIG T 0x089116eb
//   bool (char* path, AvatarRoulette&)
// [avatar coin info]/[rare clone avatar rate]/[rare avatar rate]/
// [upper avatar rate]/[normal avatar rate] 五个标量 + 十个 job 节
// ([swordman]..[DemonicSwordman])，每节 do{Detail}while(token==[/节名])。
// EOF（ScanTypeOrInt 非型且返 0）或 load 失败之外的出口恒为 true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089116eb。真语义 bug 已修：job 节 do-while 继续条件原写
// `token == "[/节名]"`（方向反，会多吃一个节头），按 ORIG 0x089118a5 `je body`
// 实证改为 `!(token == "[/节名]")`；十个 job 节为**独立 if 序列**（非 else-if）。
// 本轮：十节 do-while 改 `for(;;){Detail; if(token=="[/节名]")break;}` 复现 4.1
// 形 `jmp body; nop; body…; test %al,%al; je body`（do-while 在 4.4 恒产
// `xor $1; test; jne body`）⇒ gap 21→2（raw 328 vs 326，norm 307 vs 305）。
// 残差 2 条：① m_avatarCoinInfo 首节 store ORIG `mov %eax,%edx; mov this,%eax;
// mov %edx,(%eax)` RHS-first+中转 vs 我方 LHS-first（T1–T4 无字段类型直证，
// P321 T5 禁以装载序改类型）⇒ N76。
// [SweepInitProd9 本轮] ② 已翻正：旧判「空 else/continue 两形不闭合」被 P223
// **尾空标签**形推翻——五个标量节尾各 `goto after_sections;`（ORIG 0891179f/
// 089117cd/089117fb/08911829/08911857 的 `jmp 08911b5c`），循环体尾置
// `after_sections:;`（=ORIG 08911b5c 共享中继岛 `jmp 主循环头`）。实测：
// jcc 48=48、目标块签名差异段 6→**0**（逐位全等）；ourN 305→306、ORIG 307
// ⇒ gap 2→1。剩余唯一条 = N76 store 序（`mov %eax,%edx` 中转）。
// p167 CLEAR（47=47）。
bool importAvatarRouletteList(char* path, AvatarRoulette& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x14
    bool ok = false;                    // ORIG -0xd（死局部，ORIG 仅写一次）
    bool isType = false;                // ORIG -0x15
    bool isInt = false;                 // ORIG -0x16
    (void)ok;

    out.clear();

    for (;;)
    {
        int ret = ScanTypeOrInt(isType, token, isInt, true);   // ORIG -0xc
        if (!isInt && ret == 0)
        {
            break;
        }
        if (token == "[avatar coin info]")
        {
            out.m_avatarCoinInfoU = ScanInt((bool*)0);   // [L16] P211 档③（ORIG 0891179f 段 RHS-first）
            goto after_sections;        // ORIG 0891179f jmp 08911b5c（共享岛）
        }
        else if (token == "[rare clone avatar rate]")
        {
            out.m_rareCloneAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[rare avatar rate]")
        {
            out.m_rareAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[upper avatar rate]")
        {
            out.m_upperAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else if (token == "[normal avatar rate]")
        {
            out.m_normalAvatarRate = ScanInt((bool*)0);
            goto after_sections;
        }
        else
        {
            if (token == "[swordman]")
            {
                // ORIG 08911876 形：`body; call operator==; test %al,%al; je body`
                // （4.1 do-while 条件在尾、无 xor）⇒ 用 for(;;)+`if(cond) break;`
                // 复现直分支 je（do-while 在 4.4 恒产 `xor $1; test; jne body`）。
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)0);
                    if (token == "[/swordman]") { break; }
                }
            }
            if (token == "[fighter]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)1);
                    if (token == "[/fighter]") { break; }
                }
            }
            if (token == "[gunner]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)2);
                    if (token == "[/gunner]") { break; }
                }
            }
            if (token == "[mage]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)3);
                    if (token == "[/mage]") { break; }
                }
            }
            if (token == "[priest]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)4);
                    if (token == "[/priest]") { break; }
                }
            }
            if (token == "[at_gunner]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)5);
                    if (token == "[/at_gunner]") { break; }
                }
            }
            if (token == "[thief]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)6);
                    if (token == "[/thief]") { break; }
                }
            }
            if (token == "[at_fighter]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)7);
                    if (token == "[/at_fighter]") { break; }
                }
            }
            if (token == "[at_mage]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)8);
                    if (token == "[/at_mage]") { break; }
                }
            }
            if (token == "[DemonicSwordman]")
            {
                for (;;)
                {
                    importAvatarRouletteListDetail(out, token, (ENUM_CHARACTERJOB)9);
                    if (token == "[/DemonicSwordman]") { break; }
                }
            }
            // 注：ORIG 08911b57 节尾另有 1 条显式 `jmp 主循环头`；4.4 对链尾裸
            // if 恒 jump-thread（空 else / 显式 continue 两形实测均不闭合）⇒ N。
        }
        // b28d-solo-InitScriptProducers_6: P223 尾空标签 — ORIG 08911b5c 是
        // 五个标量节共享的 `jmp 主循环头` 中继岛；标量节尾显式 goto 至此，
        // 4.4 若线程化则本形不成立（判词记实测读数）。
    after_sections:;
    }
    return true;
}

// ============================================================================
// importDeathTowerScript —— ORIG T 0x089089f3
//   int (const char*, stDeathTower_t&)，0=成功，否则 init.md Fail(%d) 行号码。
// 八个节：单值节带错误码；四个 200 表节从下标 1 开始连续 ScanFloat/ScanInt
// 直至失败（ORIG 无边界检查，忠实保留）；未知节名 → 0x18cd 错误。
// 所有 Scan 均为「先写入字段、后校验 ok」（fstp/mov 先于 ok 判定；
// [reward item prob] 三列前缀和亦同），与 ORIG 指令顺序一致。
// ============================================================================
// b28d-21 复验 — ORIG 0x089089f3。已修：ScanType 失败走 break + 循环外 return 0
// （ORIG `jne <返回0岛>` 0x08908d0f）；表节循环 `field[i]=Scan; ++i; if(!ok)break`
// （ORIG `fstps; addl; xor; test; je body`）；[reward item prob] 三列前缀和无具名
// 中转局部。p167 CLEAR（24=24）。残差 gap 1（raw 243 vs 242，norm 234 vs 233）：
// ① 3 处 store ORIG 为 RHS-first+`mov %eax,%edx` 中转（[apc exp weight] 首节
// 08908a73 段 + [reward card num] 08908b06 段 + [illusion reward card num]
// 08908ba8 段）vs 我方 LHS-first——m_rewardItemProb/m_rewardCardNum 的 int/unsigned
// 无 T1–T4 直证，且改型需动 CDataManager.h + 4 个消费 TU（P321 T5 禁）⇒ N76；
// ② [reward item prob] 两个 += 段我方 `lea (%edx,%eax,1); mov %eax,%edx` vs ORIG
// `add %eax,%edx`（N4/P30）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 233（gap +1）；jcc 40=40 且
// **目标块签名差异段 = 0**（逐位交叉验证全等，含全部条件方向/落点）——控制流
// 与 ORIG 完全同构，残差纯为上述 N76 store 序 + N4/P30 与 1 条 nop。
int importDeathTowerScript(const char* path, stDeathTower_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x187e;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apc exp weight]")
        {
            out.m_apcExpWeight = ScanFloat(&ok);    // fstps (+0x000)，float 位模式
            if (!ok) { return 0x1887; }
        }
        else if (token == "[reward exp weight]")
        {
            int i = 1;                              // ORIG 下标从 1 开始
            for (;;)
            {
                out.m_rewardExpWeight[i] = ScanFloat(&ok);   // fstps 0x4(%eax,%ebx,4)
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[reward card num]")
        {
            int i = 1;
            for (;;)
            {
                // [L16 本轮] P211 档③ 试形：RHS 显式 unsigned 与 int 字段异型
                // ⇒ 复现 ORIG RHS-first + `mov %eax,%edx` 中转（08908b06 段）。
                out.m_rewardCardNumU[i] = ScanInt(&ok);  // (0xc8+i)*4+4 → +0x324+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward exp weight]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardExpWeight[i] = ScanFloat(&ok);  // (0x190+i)*4+4 → +0x644+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward card num]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardCardNumU[i] = ScanInt(&ok);  // P211 档③（08908ba8 段）
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[monster exp weight]")
        {
            out.m_monsterExpWeight = ScanFloat(&ok);  // fstps +0xc84
            if (!ok) { return 0x18b8; }
        }
        else if (token == "[reward item prob]")
        {
            // P211：字段 int 与 RHS unsigned 异型 ⇒ RHS-first + `mov %eax,%edx` 中转。
            out.m_rewardItemProbU[0] = ScanInt(&ok);   // ORIG 先存储后校验（08908a73 段）
            if (!ok) { return 0x18be; }
            // [L16 本轮] 书写序两形实测：`mem + call()` 反劣（ourN 236、ndiff 26）；
            // `call() + mem` 最优（ourN 232、ndiff 6，残 lea-vs-add 2 处 = N4/P205）。
            out.m_rewardItemProbU[1] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[0];
            if (!ok) { return 0x18c1; }
            out.m_rewardItemProbU[2] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[1];
            if (!ok) { return 0x18c4; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_rewardGoldWeight = ScanFloat(&ok);  // fstps +0xc94
            if (!ok) { return 0x18ca; }
        }
        else
        {
            return 0x18cd;                          // ORIG 未知节名即失败
        }
    }
    return 0;
}

// ============================================================================
// importBloodDungeonScript —— ORIG T 0x08909016
//   int (const char*, stBloodDungeon_t&)，0=成功。
// [ultimate reward prob] 以 map find 判重：key 已存在 → 0x196e 错误。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x08909016。已修：
// [ultimate reward prob] 改 `it == end()` 正极性 + `make_pair(key, prob)` 经
// pair<const int,T> converting ctor（P61/P178），p167 由 3 MISSING/2 EXTRA 转
// CLEAR（33=33）；[reward exp weight] 改 `++i` 先于 ok 检查。残差 N112（nop ×）
// + P211 store 序 + 槽（271 vs 264）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 263 / ourN 265（gap −2），jcc 36=36、
// 目标块差异 7 段（全部为 ORIG nop 中继岛/块首落点差，jcc 方向与数量逐位一致）
// ⇒ N112/P211 封顶，无控制流真反。
int importBloodDungeonScript(const char* path, stBloodDungeon_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x191c;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[reward exp weight]")
        {
            int i = 0;                              // 本表下标从 0 开始
            for (;;)
            {
                out.m_rate[i] = ScanFloat(&ok);     // ORIG fstps 成员直写、后查 ok
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[exp standard monster cnt]")
        {
            int v = ScanInt(&ok);
            out.m_field320 = v;
            if (!ok) { return 0x192f; }
        }
        else if (token == "[reward item prob]")
        {
            int v1 = ScanInt(&ok);
            if (!ok) { return 0x1936; }
            out.m_field324[0] = v1;
            int v2 = ScanInt(&ok);
            if (!ok) { return 0x1939; }
            out.m_field324[1] = out.m_field324[0] + v2;
            int v3 = ScanInt(&ok);
            if (!ok) { return 0x193c; }
            out.m_field324[2] = out.m_field324[1] + v3;
        }
        else if (token == "[reward gold weight]")
        {
            out.m_field330 = ScanFloat(&ok);        // ORIG fstps 0x330(%eax) 直写
            if (!ok) { return 0x1942; }
        }
        else if (token == "[reward exp inout count weight]")
        {
            for (;;)
            {
                float v = ScanFloat(&ok);
                if (!ok) { break; }
                out.m_vec334.push_back(v);
            }
        }
        else if (token == "[ultimate difficulty point]")
        {
            out.m_ultDiffPoint344 = ScanInt((bool*)0);
            out.m_ultPointTotal348 = ScanInt((bool*)0);
        }
        else if (token == "[ultimate reward prob]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                STUltimateRewardProb prob;
                prob.m_field0 = ScanInt((bool*)0);
                prob.m_field4 = ScanInt((bool*)0);

                std::map<int, STUltimateRewardProb>::iterator it =
                    out.m_map358.find(key);
                if (it == out.m_map358.end())
                {
                    out.m_map358.insert(std::make_pair(key, prob));
                }
                else
                {
                    return 0x196e;                  // ORIG key 重复即失败
                }
            }
        }
    }
}

// ============================================================================
// importCharacLinkSystemScript —— ORIG T 0x08909529
//   int (const char*, stCharacLinkSystem_t&)，0=成功。
// [link level] 写 +0x00；四个 bonus 节各自选定目标 vector 后 do-while 连续装
// 4×ScanInt 记录；未匹配节名静默跳过（ORIG al=0 → 回外层）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08909529 — 第二个 string 局部
// unused(-0x18，仅 ctor/dtor)；[link level] ScanInt 直存无局部；四 bonus 节
// 合并 `||` 短路链后**四路** else-if 选 target（第 4 路仍比较，无裸 else）。
int importCharacLinkSystemScript(const char* path, stCharacLinkSystem_t& out)
{
    bool ok = false;
    std::string token;                  // ORIG -0x14
    std::string unused;                 // ORIG -0x18（仅构造/析构，零引用）

    if (!loadRDARScriptFile("", path))
    {
        return 0x19ab;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[link level]")
        {
            out.m_field0 = ScanInt(&ok);
            if (!ok) { return 0x19b7; }
        }
        else if (token == "[low link character exp bonus]" ||
                 token == "[low link character gold bonus]" ||
                 token == "[high link character exp bonus]" ||
                 token == "[high link character gold bonus]")
        {
            std::vector<stCharacLinkSystemData>* target = 0;
            if (token == "[low link character exp bonus]")
            {
                target = &out.m_vec4;               // +0x04
            }
            else if (token == "[low link character gold bonus]")
            {
                target = &out.m_vec10;              // +0x10
            }
            else if (token == "[high link character exp bonus]")
            {
                target = &out.m_vec1c;              // +0x1c
            }
            else if (token == "[high link character gold bonus]")
            {
                target = &out.m_vec28;              // +0x28
            }

            for (;;)
            {
                stCharacLinkSystemData data;        // ORIG ctor 0x891b762 四连清零
                data.m_field0 = ScanInt(&ok);
                if (!ok) { break; }                 // ORIG 此路径跳过 dtor，无观测差异（trivial dtor）
                data.m_field4 = ScanInt(&ok);
                if (!ok) { return 0x19d5; }
                data.m_field8 = ScanInt(&ok);
                if (!ok) { return 0x19da; }
                data.m_fieldc = ScanInt(&ok);
                if (!ok) { return 0x19df; }
                target->push_back(data);
            }
        }
    }
    (void)unused;
    return 0;
}

// ============================================================================
// importCharacterCreateGiftScript —— ORIG T 0x0895eda1
//   bool (createChracScript*, const char*)
// [apply job] 记忆当前 job（getCharacterJobToEnumValue）；[/apply job] 把累积的
// rewardInfo 挂入 m_map0[job] 并复位累积器；[/create character gift] 返回 true；
// EOF 未遇结束符 → false；时间串转换失败 → false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: IDENTICAL（634=634）— 清单标注 NEAR2 为旧读数；
// 本波 fresh 复验已闭合（双相派发 + 循环内静态三件套 P177 形）。ORIG T 0x0895eda1。
bool importCharacterCreateGiftScript(createChracScript* s, const char* path)
{
    // ORIG 双相派发（T 0x0895eda1）：函数级静态三件套位于主循环体内（ORIG
    // +0x8b/+0x107 每轮穿 guard），case 贯穿注册链按
    // [/create character gift]0x1ab→[apply job]0x1af→[item]0x1b5→
    // [new account event start time]0x1bc→[end time]0x1c2→[create title]0x1d2→
    // [create text]0x1d6→[/apply job]0x1da；执行形 0x1ab 段 = return true，
    // 时间转换 ret!=1 段 return false，EOF 出口 `movzbl -0x1d,%ebx` 读 ok 槽
    // （全函数无写入 ⇒ 恒 false）。
    // 收敛口径：orig=ours=637，助记符序列全等；残余仅 .o 未链接的 guard/静态
    // 对象/串字面量重定位显示（_ZZ…E8__lookup/11__init_lock/8__inited + 双
    // _ZGV guard 与 ORIG 同名，8 个 tag 串 .rodata 内容实测逐条一致），
    // 链接后按 AE 口径判等。
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                            // ORIG -0x108
    bool ok = false;                              // ORIG -0x1d（+0x4e 写，仅 EOF 出口读）
    createChracScript::rewardInfo info;           // ORIG -0x124
    int curJob = -1;                              // ORIG -0x128

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x1ab:                      // [/create character gift]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[/create character gift]", 0x1ab));
                }
                else
                {
                    return true;              // ORIG +0x35c mov$1,%ebx
                }
                /* fall through（ORIG +0x35a jmp +0x366） */
            case 0x1af:                      // [apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[apply job]", 0x1af));
                }
                else
                {
                    std::string jobName;      // ORIG -0x130
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（+0x3c1 jmp +0x421） */
            case 0x1b5:                      // [item]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item]", 0x1b5));
                }
                else
                {
                    int itemId = 0;           // ORIG -0x134（movl $0 先行）
                    ScanInt(&itemId);
                    info.m_items.push_back(itemId);
                    break;
                }
                /* fall through（+0x4ae jmp +0x4b3） */
            case 0x1bc:                      // [new account event start time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event start time]", 0x1bc));
                }
                else
                {
                    std::string str;          // ORIG -0x138
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_startTime) != 1)
                        return false;         // ORIG +0x597 mov$0,%ebx 出口
                    break;
                }
                /* fall through（+0x50e jmp +0x5e0） */
            case 0x1c2:                      // [new account event end time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event end time]", 0x1c2));
                }
                else
                {
                    std::string str;          // ORIG -0x13c
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_endTime) != 1)
                        return false;
                    break;
                }
                /* fall through（+0x63b jmp +0x70d） */
            case 0x1d2:                      // [create title]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create title]", 0x1d2));
                }
                else
                {
                    ScanStr(&info.m_title);   // ORIG info+0x14 直写
                    break;
                }
                /* fall through（+0x75c jmp +0x774） */
            case 0x1d6:                      // [create text]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create text]", 0x1d6));
                }
                else
                {
                    ScanStr(&info.m_text);    // ORIG info+0x18 直写
                    break;
                }
                /* fall through（+0x7c0 jmp +0x7d8） */
            case 0x1da:                      // [/apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/apply job]", 0x1da));
                }
                else
                {
                    if (curJob != -1)
                    {
                        std::map<int, createChracScript::rewardInfoList>::iterator it =
                            s->m_map0.find(curJob);
                        if (it == s->m_map0.end())
                        {
                            createChracScript::rewardInfoList list;
                            list.m_list.push_back(info);
                            s->m_map0.insert(std::make_pair(curJob, list));
                        }
                        else
                        {
                            it->second.m_list.push_back(info);
                        }
                    }
                    info.m_items.clear();     // ORIG +0x961..+0x9d8 复位六连
                    info.m_title.clear();
                    info.m_text.clear();
                    info.m_startTime = 0;
                    info.m_endTime = 0;
                    curJob = -1;
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_0895f783 公共尾。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return ok;                               // ORIG EOF 尾岛 movzbl -0x1d,%ebx
}

// ============================================================================
// importCharacterCreateCreatureGiftScript —— ORIG T 0x0896001a
//   bool (CharacterCreateCreatureGiftScript*, const char*)
// ORIG 双相派发（与 importTitleBookEtcScript/importAttendanceScript 同构）：
//   函数级静态 __lookup(map<const char*,int,less_TCHAR>)/__init_lock(SpinLock)/
//   __inited(bool) 三件套位于外层 token 循环体内（每轮穿过 guard 测试，
// ORIG +0x8e/+0x108），注册完成 SpinLock::leave + __inited=1 后 jmp +0x8e 以
// 当前令牌重查表派发。case 贯穿链按 [event job]→[start]→[end]→[/event job]→
// [change able creature lev]→[change infinity creature]→[client infinity
// creature] 顺序 insert(0x277/0x27d/0x281/0x285/0x28c/0x290/0x2a2)。
// 收敛口径：orig=ours=487，助记符序列全等、栈槽逐条一致；残余差异仅为
// .o 未链接的 guard/静态对象/串字面量重定位显示（.rodata 内容与 5 个局部
// 静态符号名 _ZZ…E8__lookup/11__init_lock/8__inited+双 guard 实测与 ORIG
// 一致），链接后按 AE 口径判等。
// ============================================================================
// b28d-solo-InitScriptProducers_6: IDENTICAL（482=482）— 清单标注 NEAR2 为旧读数；
// 本波 fresh 复验已闭合（P164 EOF 落点形）。ORIG T 0x0896001a。
bool importCharacterCreateCreatureGiftScript(CharacterCreateCreatureGiftScript* s,
                                             const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == 0)
    {
        return false;
    }

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;                       // ORIG -0x10d
    int curJob = -1;                       // ORIG -0x114
    CharacterCreateCreatureGiftScript::eventJob eventJobItem;   // ORIG -0x11c

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        // ORIG 重派发跳回 +0x8e（__lookup guard）：静态声明位于循环体内。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x277:                      // [event job]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[event job]", 0x277));
                }
                else
                {
                    std::string jobName;    // ORIG -0x124 每轮构造/析构
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（ORIG +0x342 jmp +0x3a2） */
            case 0x27d:                      // [start]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[start]", 0x27d));
                }
                else
                {
                    ScanInt(&eventJobItem.m_minLevel);   // ORIG -0x11c
                    break;
                }
                /* fall through（+0x3fd jmp +0x412） */
            case 0x281:                      // [end]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[end]", 0x281));
                }
                else
                {
                    ScanInt(&eventJobItem.m_maxLevel);   // ORIG -0x11c+4
                    break;
                }
                /* fall through（+0x46d jmp +0x485） */
            case 0x285:                      // [/event job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event job]", 0x285));
                }
                else
                {
                    s->m_eventJobMap.insert(std::make_pair(curJob, eventJobItem));
                    eventJobItem.m_minLevel = -1;        // ORIG 复位三连
                    eventJobItem.m_maxLevel = -1;
                    curJob = -1;
                    break;
                }
                /* fall through（+0x4e0 jmp +0x567） */
            case 0x28c:                      // [change able creature lev]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change able creature lev]", 0x28c));
                }
                else
                {
                    ScanInt(&s->m_changeAbleCreatureLev);  // ORIG +0x18
                    break;
                }
                /* fall through（+0x5b6 jmp +0x5cb） */
            case 0x290:                      // [change infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change infinity creature]", 0x290));
                }
                else
                {
                    while (true)
                    {
                        int first = 0;                   // ORIG -0x128
                        int second = 0;                  // ORIG -0x12c
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_clientChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through（+0x617 jmp +0x6d5） */
            case 0x2a2:                      // [client infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[client infinity creature]", 0x2a2));
                }
                else
                {
                    while (true)
                    {
                        int second = 0;                  // ORIG -0x130（先声明）
                        int first = 0;                   // ORIG -0x134（后声明）
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_infinityChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089607f4 公共尾：注册链走完释放锁并置位，重派发当前
            // 令牌；正常解析态直接 ScanType 下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return true;                           // ORIG EOF → ebx=1
}

// ============================================================================
// importConditionEventChkDungeonScript —— ORIG T 0x08913891
//   bool (const char*, ConditionLevelChkDungeon*)
// [apply level] 写 +0x0c；[unable dungeon] 清空后 do-while push_back；
// 无 unloadRDARScriptFile（ORIG 如此）；out 为空指针时返回 false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08913891 — load 检查在最前，之后按
// token(-0x10)/unused(-0x14，仅 ctor/dtor 零引用，P171)/ok(-0x15) 声明；
// 循环出口用 break（ORIG `jne exit`）而非 return true。
bool importConditionEventChkDungeonScript(const char* path,
                                          ConditionLevelChkDungeon* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apply level]")
        {
            int v = ScanInt(&ok);
            out->m_applyLevel = v;                  // +0x0c
        }
        else if (token == "[unable dungeon]")
        {
            out->m_unableDungeons.clear();
            for (;;)
            {
                int dungeonIdx = ScanInt(&ok);
                if (!ok) { break; }
                out->m_unableDungeons.push_back(dungeonIdx);
            }
        }
    }
    (void)unused;
    return true;
}

// ============================================================================
// importConditionEventScript —— ORIG T 0x0890e868
//   bool (const char*, stConditionEventInfo*)
// [id]→+0x00；[condition match]→map+0x08（key,(v1,v2)）；
// [condition reward]→multimap+0x20；[condition quick party add reward]→
// multimap+0x38。两条 reward 节均扫 1 key+4 值，末两值 setne 成 bool。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890e868。已修：load 检查
// 最前，token(-0xa0)/unused(-0xa4)/ok(-0xa5) 置于 load 之后。p167 CLEAR（39=39）、
// 323=323、助记符序列剥离 nop 后全等；残差仅 N112（循环入口/失败岛 nop ×3）⇒ N。
bool importConditionEventScript(const char* path, stConditionEventInfo* out)
{
    // ORIG T 0x0890e868（323 条；orig=ours=323，助记符序列仅 4 处 4.1 块放置
    // 顺序差：entry jmp/nop landing 前置与 EOF dtor 块位——跨版本 CFG 布局，
    // 无计数/调用/操作数缺口）。四 token 段 reward/quick 为**各自独立展开**
    // （ORIG 槽组 -0xbc/-0x28..-0x1c vs -0xc8/-0x18..-0xc 两套即证）；map/
    // multimap 提交为 make_pair(lvalue)+pair 转换 ctor+insert 三段 call 形
    // （P61/P123，value_type 直构会少一段）；[id] 节 ScanInt 失败 → break
    // 主循环 → return true；主循环外还有一个仅构造/析构的 dead string
    // （ORIG -0xa4）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                 // ORIG -0xa0
    std::string unused;                // ORIG -0xa4（ctor/dtor 各一次，零引用）
    bool ok = false;                   // ORIG -0xa5

    for (;;)
    {
        if (!ScanType(token, true))
        {
            goto loop_exit;                        // 共享 true 出口岛（ORIG +0x4b2）
        }

        if (token == "[id]")
        {
            int v = ScanInt(&ok);
            if (ok != true)
            {
                goto loop_exit;
            }
            // [L16 本轮] 唯一残差点（NEAR ndiff=2）：ORIG `mov v,%edx; mov out,%eax;
            // mov %edx,(%eax)` = RHS-first ⇒ RHS 异型化（P211 档③）。
            out->m_field0U = v;                    // ORIG mov %edx,(%eax) → +0x00
        }
        else if (token == "[condition match]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // ORIG -0xac
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x30
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x2c
                if (ok != true) { break; }
                std::pair<int, int> value;         // 默认构造后 first/second 赋值（ORIG +0x14a 序）
                value.first = v1;
                value.second = v2;
                out->m_map8.insert(std::make_pair(key, value));
            }
        }
        else if (token == "[condition reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xb8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x28
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x24
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x20
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0x1c
                if (ok != true) { break; }
                stConditionEventRewardInfo info;   // 默认构造（ORIG 0891b682 每轮）
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);          // cmpl$0;setne 直存
                info.m_flag9 = (v4 != 0);
                out->m_multiMap20.insert(std::make_pair(key, info));   // +0x20
            }
        }
        else if (token == "[condition quick party add reward]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);            // -0xc8
                if (ok != true) { break; }
                int v1 = ScanInt(&ok);             // -0x18
                if (ok != true) { break; }
                int v2 = ScanInt(&ok);             // -0x14
                if (ok != true) { break; }
                int v3 = ScanInt(&ok);             // -0x10
                if (ok != true) { break; }
                int v4 = ScanInt(&ok);             // -0xc
                if (ok != true) { break; }
                stConditionEventRewardInfo info;
                info.m_field0 = v1;
                info.m_field4 = v2;
                info.m_flag8 = (v3 != 0);
                info.m_flag9 = (v4 != 0);
                out->m_multiMap38.insert(std::make_pair(key, info));   // +0x38
            }
        }
    }
loop_exit:
    return true;
}

// ============================================================================
// importDBLogItem —— ORIG T 0x0890dbba
//   bool (char*, std::vector<stDBLogItem>*)
// 外层每轮消耗两个节头：第一 ScanType 匹配 [dblog buyitem]（flag=0 批量）、
// 第二匹配 [dblog getitem]（flag=1）；记录={id,ConvertStringToTime(str),flag}，
// id ScanInt 失败即节尾。任一 ScanStr/时间转换失败 → 整体 false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890dbba。已修：删两处
// `std::string tmp(str)` 中转，直接 `ConvertStringToTime(str, ...)`（by-value 形参
// 自身拷贝即可），p167 由 6 EXTRA 转 CLEAR（28=28）。
// [SweepInitProd9 本轮] ORIG 0x0890dbba 逐位复验 + 三处 P0/P171 修复：
// ① `int id = ScanInt(&ok); item.m_itemIdx = (unsigned)id;` → 内联
//    `item.m_itemIdx = (unsigned)ScanInt(&ok);`（ORIG 0890dc70/0890dd82
//    `mov %eax,-0x34/-0x40` 直存，无 id 槽往返）——删 2×2 条；
// ② `bool convOk = (ConvertStringToTime(...) == 1); if (!convOk)` → 裸
//    `if (!ConvertStringToTime(...))`（ORIG 0890dcc5 `mov %eax,%ebx; xor $1,%ebx`
//    无 bool 槽）——删 2×1 条；
// ③ 补 ORIG 三处死写入（P171）：0890dc03 `movl $0x0,-0x14`（prologue）、
//    0890dc43 `movl $0x0,-0x10`（buyitem 入口）、0890dd55 `movl $0x0,-0xc`
//    （getitem 入口）——补 3 条；声明序按 ORIG 槽浅→深排 deadA/token/str/ok，
//    result（4.4 共享尾替代 ORIG 的 ebx 承载）声明最后。
// 读数：ourN 202→199 = ORIG 199（gap 0）；jcc 28/27（ORIG 尾部 EOF 岛
// 0890de2a `jmp 0890de2d` 被 4.4 jump-thread，P223 尾标签法在此跨非平凡
// 局部声明不可用）+ 8 段目标块序差（块首 string-ctor 块 vs memset 块）+ 槽整体
// 深 4（result 槽，ORIG 用 ebx）⇒ N82/N95/P223 封顶。
bool importDBLogItem(char* path, std::vector<stDBLogItem>* out)
{
    // ORIG T 0x0890dbba（204 条；单出口 ebx 形——4.1 把所有失败/EOF 路径 merge
    // 到共享 dtor+return 链，4.4 的多 return 形会复制 3 组 string dtor ≈ +52。
    // 本形以 result 局部 + goto done 复现共享出口）。外层每轮消耗两个节头：
    // head1=[dblog buyitem]（flag 0）内层循环、head2=[dblog getitem]（flag 1）
    // 内层循环；内层 ScanInt 断流回下一节头/主循环顶；ScanStr/时间转换失败
    // 整体 false；两处 EOF 均 true。ConvertStringToTime 的 by-value 串参以块级
    // 临时物化、dtor 先于结果测试（ORIG +0x106..+0x11b 序）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    // b28d-solo-InitScriptProducers_6: P171 死写入照抄 — ORIG 0890dc03
    // `movl $0x0,-0x14(%ebp)`（prologue 后零写、全函数无读取）。声明序按
    // ORIG 槽浅→深（4.4 先声明占浅槽，P212/P331）：deadA(-0x14)→token(-0x20)
    // →str(-0x24)→ok(-0x25)；result 为 4.4 共享尾的替代物（ORIG 用 ebx 承载），
    // 声明在最后使其落最深槽、不挤占上述 ORIG 槽位。
    int deadA = 0;
    (void)deadA;
    std::string token;                 // ORIG -0x20
    std::string str;                   // ORIG -0x24
    bool ok = false;                   // ORIG -0x25
    bool result = true;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[dblog buyitem]")
        {
            // b28d-solo-InitScriptProducers_6: P171 — ORIG 0890dc43
            // `movl $0x0,-0x10(%ebp)`（分支入口零写、循环回边不经过、无读取）。
            int deadB = 0;
            (void)deadB;
            for (;;)
            {
                stDBLogItem item;                  // ORIG memset(&item,0,0xc)
                memset(&item, 0, sizeof(item));
                // b28d-solo-InitScriptProducers_6: P0/P183 — ORIG 0890dc70
                // `mov %eax,-0x34(%ebp)` 直存字段（无具名 id 槽往返），ScanInt
                // 结果内联进字段赋值。
                item.m_itemIdx = static_cast<unsigned int>(ScanInt(&ok));
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                // b28d-solo-InitScriptProducers_6: P168a — ORIG 0890dcc5
                // `mov %eax,%ebx; xor $0x1,%ebx`（bool 取反入 ebx 跨 dtor 保活，
                // 无 bool 槽）⇒ 裸 `!ConvertStringToTime(...)` 形。
                if (!ConvertStringToTime(str, item.m_time))
                {
                    result = false;
                    goto done;
                }
                item.m_value = 0;                  // +0x08 memset 冗余重写 0
                out->push_back(item);
            }
        }

        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[dblog getitem]")
        {
            // b28d-solo-InitScriptProducers_6: P171 — ORIG 0890dd55
            // `movl $0x0,-0xc(%ebp)`（分支入口零写、无读取）。
            int deadC = 0;
            (void)deadC;
            for (;;)
            {
                stDBLogItem item;
                memset(&item, 0, sizeof(item));
                // b28d-solo-InitScriptProducers_6: 同 buyitem（ORIG 0890dd82 直存）。
                item.m_itemIdx = static_cast<unsigned int>(ScanInt(&ok));
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                if (!ConvertStringToTime(str, item.m_time))
                {
                    result = false;
                    goto done;
                }
                item.m_value = 1;                  // +0x08 写 1
                out->push_back(item);
            }
        }
    }
done:
    return result;
}


// ============================================================================
// importDailyScheduleEtcScript —— ORIG T 0x0891826c
//   int (const char*, std::vector<_DailySchedule>&)，0=成功。
// [schedule] 内 do-while 连续装配 _DailySchedule（0x24）：3×ScanInt +
// 5×ScanStr（后四者失败带 0x3721..0x3737 行号码）+ 两次 ConvertStringToTime
// （+0x0c/+0x10）+ weekday（+0x14）+ 两组 ConvertHHMMSSStringToInt（+0x18..）。
// ============================================================================
int importDailyScheduleEtcScript(const char* path,
                                 std::vector<_DailySchedule>& out)
{
    // ORIG 0891826c：loadRDARScriptFile 先于任何局部构造（失败直接 ret 0x36f7，
    // 无 string 析构），随后才 basic_string() token（8918298）→ -0x14 置 0
    // （89182a3）→ ok=false（89182aa）。
    if (!loadRDARScriptFile("", path))
    {
        return 0x36f7;
    }

    std::string token;                              // ORIG -0x2c
    int nUnusedCounter = 0;                         // ORIG -0x14：仅 89182a3 置 0，全函数无其它引用
    bool ok = false;                                // ORIG -0x2d
    (void)nUnusedCounter;

    // ORIG 89182c9：ScanType 失败是 break 出外层循环（跳到 8918782 的
    // nop/mov $0x0,%ebx 尾块），不是就地 return。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[schedule]")
        {
            // ORIG 89182e6..8918318：5 个 std::string 在 [schedule] 分支入口
            // 一次性默认构造（-0x34/-0x38/-0x3c/-0x40/-0x44），循环体内复用，
            // 循环出口 89186a6.. 统一析构；不在每轮迭代内重建。
            std::string strStart;                   // -0x34
            std::string strEnd;                     // -0x38
            std::string strWeekday;                 // -0x3c
            std::string strStartTime;               // -0x40
            std::string strEndTime;                 // -0x44

            for (;;)
            {
                _DailySchedule entry;               // ORIG -0x6c
                memset(&entry, 0, sizeof(entry));   // 8918333 memset(...,0,0x24)

                // ORIG 891833e/891835b/891837e：第 1、3 枚 ScanInt 先落局部
                // （-0x10/-0xc）再回填字段，第 2 枚直接写 entry+0x04。
                int nEventId = ScanInt(&ok);        // -0x10
                if (!ok) { break; }                 // 三枚普通 ScanInt 失败仅回外层
                entry.m_type = ScanInt(&ok);        // +0x04（直接写字段）
                if (!ok) { break; }
                entry.m_eventId = nEventId;         // +0x00（8918372）
                int nIdx = ScanInt(&ok);            // -0xc
                if (!ok) { break; }
                entry.m_idx = nIdx;                 // +0x08（8918395）

                if (!ScanStr(&strStart))
                {
                    break;                          // 首串失败同样回外层（ORIG 89183ab）
                }
                if (!ScanStr(&strEnd))
                {
                    return 0x3721;
                }
                if (!ScanStr(&strWeekday))
                {
                    return 0x3723;
                }
                if (!ScanStr(&strStartTime))
                {
                    return 0x3725;
                }
                if (!ScanStr(&strEndTime))
                {
                    return 0x3727;
                }

                // ORIG 8918435/89184a4/8918513/891857e/89185fa：按值形参的
                // std::string 拷贝直接由实参构造（唯一一次 _ZNSsC1ERKSs），
                // 不存在额外的具名 tmp 中转。
                if (!ConvertStringToTime(strStart, *(long*)&entry.m_startSec))
                {
                    return 0x372a;                  // +0x0c
                }
                if (!ConvertStringToTime(strEnd, *(long*)&entry.m_endSec))
                {
                    return 0x372c;                  // +0x10
                }
                unsigned int wday = 0;              // ORIG -0x48（8918505）
                if (!ConvertWeekdayStringToInt(strWeekday, wday))
                {
                    return 0x3730;                  // +0x14
                }
                entry.m_wday = static_cast<int>(wday);
                // ORIG 8918589..89185a7：三个出参直接取 entry+0x18/0x19/0x1a
                if (!ConvertHHMMSSStringToInt(strStartTime, entry.m_startHour,
                                              entry.m_startMin, entry.m_startSec2))
                {
                    return 0x3735;
                }
                // ORIG 8918605..891861f：entry+0x1b/0x1c/0x1d
                if (!ConvertHHMMSSStringToInt(strEndTime, entry.m_endHour,
                                              entry.m_endMin, entry.m_endSec2))
                {
                    return 0x3737;
                }

                out.push_back(entry);               // 回 do 头继续下一记录
            }
        }
    }

    return 0;                                       // ORIG 8918783 mov $0x0,%ebx
}

// ============================================================================
// importDimensionActivationData —— ORIG T 0x89160f0
// ORIG 结构（/tmp/importDimAct.md 汇编+Ghidra C 双段实证）：
// 外层节名 = 函数级静态 __lookup map<char const*,int,less_TCHAR> +
// __init_lock(SpinLock) + __inited 双检贯穿 insert 建表链（5 条：
// "[play day count]"0x3488→"[coin info]"0x348d→"[section]"0x34b7→
// "[select group]"0x34ca→"[give group]"0x34ff），建表后 find 派发；
// coin info 难度 = 第二张静态 __lookup_0（4 条：[easy]0x3499→0、
// [medium]0x349e→1、[hard]0x34a3→2、[ultimate]0x34a8→3、未命中保持
// 入口 -1 且**照常**执行公共尾 insert），两表键均 less_TCHAR 大小写无关。
// 数据落点：[play day count]→out+0x00（ScanInt 返回值直写，不查 flag）；
// coinMap(+0x04) insert(coinType, DimensionCoinInfo{+0=首 ScanInt int、
// +4=二 ScanInt!=0 的 setne 字节})；[section] 装配 EventRewardSection 入
// m_rewardData.m_vec0(+0x1c)；[select group] 按 makeFairPVPEquipmentPackageKey
// (getCharacterJob,idx) 入 m_mapC(+0x28)；[give group] 按 (job,growIdx) 入
// m_map24(+0x40)。节内 ScanStr 失败 → return false；EOF(ScanType 失败) → true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089160f0（0x0890c1f0 段为同 TU 邻居）。p167 CLEAR（175=175）。
// 本轮修真语义 bug + 归位形态，指令数 1143 vs 1122 → 1143 vs 1128（norm 1127 vs 1128）：
// ① **[section] 循环条件方向反**（真 bug）：ORIG 089169da `movzbl ok; xor $1; test;
//    je 记录块` ⇒ 原码 `if (!ok) break;`；旧写 `if (ok) break;` 在成功读到记录时反而
//    停止读 m_field1/m_field2/count/items。
// ② 9 处静态表守卫改负极性 `if (!__inited) { insert } else { 派发 }`（ORIG 089163b5/
//    0891642a/08916958/08916ac7/08916fa6/089166e9/08916755/089167c1/0891682a 均
//    `movzbl; xor $1; test; je 派发`）⇒ 各补回 1 条 xor + 1 条块尾 jmp。
// ③ 两张表的 guard 循环头改正极性 `if (__inited) { find } else { 锁 }`（ORIG 08916272/
//    089165b1 `movzbl; test; je 锁`，无 xor）。
// ④ 两处公共尾改 `if (!__inited) { leave; inited=1; continue; } break;`（ORIG 08916891/
//    0891735b `movzbl; xor $1; test; je …`）。
// ⑤ `int v = 0;` 单变量跨节复用（ORIG Ghidra local_1ac@-0x1a8：prologue 清零 +
//    coin-info used + section count + give-group value），旧拆 4 个局部致全帧槽号漂移。
// ⑥ 两串声明移到 load 检查之后（ORIG 08916130/0891613e ctor 在 load 后）；EOF 改
//    `break` + 循环外 `return true`（P164 尾部共享块）；`int first;` 去预清；
//    give-group 死存补具名 `dead20`（ORIG 0891710f `mov %eax,-0x1c`）。
// 残差 1 条（我方多）：块序差异（items 构造/tokenizeStr 段位置）+ 槽号整体深 4
//（N95/N126 版本首槽基址常数）+ 1 条尾 jmp ⇒ N82/N95 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 1143 / ourN 1141（gap +2）；jcc 155/156、
// 目标块差异 9 段（delete/insert 成对，均为静态表守卫块与 [section]/[give group]
// 内层循环的 nop 中继岛落点差，jcc 方向无互换）⇒ 维持 N82/N95，无真反。
// [SweepInitProd9 本轮·回边方向审计] ORIG backward 16 / 我方 14：差的 2 条 = 4.4
// 把 4 条 continue（leave 点 0x494d/0x4c8b/0x4e99/0x5960）合并到 2 个共享尾块
// （0x5974→jmp 4773=+0x8e 主表 guard 头；0x4ea6→jmp 4ab1=+0x3cc 子表 guard 头），
// ORIG 为 4 条独立 jmp 直达同目标 ⇒ **无回边丢失、无「退出循环」误写**。
bool importDimensionActivationData(const char* path,
                                   DimensionActivationData* out)
{
    // ORIG 实证两张函数级静态表同为 map<char const*,int,less_TCHAR>。
    typedef std::map<const char*, int, less_TCHAR> DimSectionMap;
    typedef std::map<const char*, int, less_TCHAR> DimCoinDiffMap;

    if (out == 0)
    {
        return false;                   // ORIG 0x89160fc 先于 load 判空
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    // [L16 本轮] 槽基对齐试验（P232 两个 char[4] 死局部）：前 99 条槽逐条对齐、
    // ndiff 1181→623，但第 99 条起仍差 8 ⇒ 未闭合且台账四数不变，已回滚（判 N126/N95）。
    // b28d-21: ORIG 08916130/0891613e 两串 ctor 在 load 检查之后（声明位置即
    // ctor 发射点）——旧版声明在判空之前，序言 8 条整体前移。
    std::string token;                  // ORIG -0x19c（节名，ctor 首位）
    std::string sub;                    // ORIG -0x1a0（子串）
    bool ok = false;                    // ORIG -0x1a1（ScanInt 出错 flag，两串后写）
    // ORIG -0x1a8：Ghidra local_1ac 在 prologue 清零，并被 [coin info] used、
    // [section] count、[give group] value 三处**复用**（源码级同一具名变量，
    // 非编译器槽复用）——拆成多个局部会令全帧槽号漂移。
    int v = 0;

    for (;;)                            // ORIG LAB_0891615d 主循环
    {
        if (!ScanType(token, true))
        {
            // P164：`return true` 会被 4.4 inline 到检查点后（多 je+mov+jmp）；
            // 改 break + 循环外 return true 复现 ORIG 尾部共享块（08917387 jne 直达）。
            break;                      // ORIG 0x8917388 EOF → 1
        }
        for (;;)                        // ORIG LAB_0891617e guard 重试
        {
            static DimSectionMap __lookup;      // ORIG ...E8__lookup
            static SpinLock __init_lock;        // ORIG ...E11__init_lock
            static bool __inited = false;       // ORIG ...E8__inited
            int idx;
            // ORIG 08916272 `movzbl __inited; test; je 锁路径`（无 xor）⇒ P168a②
            // 正极性 + 交换分支（`!__inited`/`==0` 均多一条 xor 物化）。
            if (__inited)
            {
                DimSectionMap::const_iterator it = __lookup.find(token.c_str());
                idx = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;           // ORIG 0x891635e 回 guard 重检
                }
                idx = 0;
            }
            switch (idx)
            {
            case 0:
            case 0x3488:                // [play day count]
                // b28d-21: ORIG 089163b5 `movzbl __inited; xor $1; test; je 派发块`
                // ⇒ 原码负极性 `if (!__inited) { insert } else { 派发 }`（P168a②）。
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[play day count]", 0x3488));
                }
                else
                {
                    out->m_field0 = ScanInt(&ok);  // ORIG 不查 flag
                    break;
                }
                /* fall through */
            case 0x348d:                // [coin info]
                // b28d-21: ORIG 0891642a 同形（`movzbl __inited; xor $1; test; je 派发`）。
                if (!__inited)
                {
                    __lookup.insert(DimSectionMap::value_type("[coin info]", 0x348d));
                }
                else
                {
                    for (;;)
                    {
                        if (!ScanStr(&sub))
                        {
                            break;      // ORIG 0x89164a0 回主循环
                        }
                        int coinType = -1;           // ORIG local_1b4 初值
                        DimensionCoinInfo coinInfo;  // ctor 0x891bcb6（查表前）
                        for (;;)                     // ORIG LAB_089164bd
                        {
                            static DimCoinDiffMap __lookup_0;    // ...__lookup_0
                            static SpinLock __init_lock_0;       // ...__init_lock_0
                            static bool __inited_0 = false;      // ...__inited_0
                            int diff;
                            // ORIG 089165b1 同形（movzbl; test; je，无 xor）。
                            if (__inited_0)
                            {
                                DimCoinDiffMap::const_iterator it2 =
                                    __lookup_0.find(sub.c_str());
                                diff = (it2 != __lookup_0.end())
                                           ? it2->second : -1;
                            }
                            else
                            {
                                __init_lock_0.enter();
                                if (__inited_0)
                                {
                                    __init_lock_0.leave();
                                    continue;
                                }
                                diff = 0;
                            }
                            switch (diff)
                            {
                            case 0:
                            case 0x3499:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[easy]", 0x3499));
                                }
                                else
                                {
                                    coinType = 0;
                                    break;
                                }
                                /* fall through */
                            case 0x349e:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[medium]", 0x349e));
                                }
                                else
                                {
                                    coinType = 1;
                                    break;
                                }
                                /* fall through */
                            case 0x34a3:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[hard]", 0x34a3));
                                }
                                else
                                {
                                    coinType = 2;
                                    break;
                                }
                                /* fall through */
                            case 0x34a8:
                                if (!__inited_0)
                                {
                                    __lookup_0.insert(
                                        DimCoinDiffMap::value_type("[ultimate]", 0x34a8));
                                }
                                else
                                {
                                    coinType = 3;
                                    break;
                                }
                            }
                            // b28d-21: ORIG 08916891 `movzbl __inited_0; xor $1;
                            // test; je 读字段` ⇒ 原码负极性（未建表则 leave+置位+回查）。
                            if (!__inited_0)
                            {
                                __init_lock_0.leave();
                                __inited_0 = true;
                                continue;
                            }
                            break;  // 未命中保持 -1，仍走公共尾 insert
                        }
                        coinInfo.m_field0 = ScanInt(&ok);        // -0x1b8
                        v = ScanInt(&ok);                        // ORIG -0x1a8
                        coinInfo.m_field4 = (v != 0) ? 1 : 0;    // setne 字节
                        // ORIG 0x8921dcd/0x8921e14：make_pair<int,DimensionCoinInfo>
                        // （按值实例化）+ value_type 转换 ctor 两级临时。
                        out->m_coinMap.insert(
                            std::map<int, DimensionCoinInfo>::value_type(
                                std::make_pair<int, DimensionCoinInfo>(
                                    coinType, coinInfo)));
                    }
                    break;
                }
                /* fall through */
            case 0x34b7:                // [section]
                if (!__inited)
                {
                    __lookup.insert(DimSectionMap::value_type("[section]", 0x34b7));
                }
                else
                {
                    for (;;)
                    {
                        EventRewardSection section;   // ctor 0x89154ee
                        section.m_field0 =
                            static_cast<unsigned char>(ScanInt(&ok));
                        // b28d-21: ORIG 0x89169e0 `movzbl ok; xor $1; test; je 记录块`
                        // ⇒ 源码为负极性 `if (!ok) break;`。旧写 `if (ok) break;`
                        // 方向反（成功读到记录时反而不读后续字段）——真语义 bug。
                        if (!ok)
                        {
                            break;      // ORIG 0x89169e8 ok 假 → 结束节块
                        }
                        section.m_field1 = ScanInt(static_cast<bool*>(0));
                        section.m_field2 = ScanInt(static_cast<bool*>(0));
                        v = ScanInt(static_cast<bool*>(0));      // ORIG -0x1a8（复用）
                        for (int i = 0; i < v; ++i)
                        {
                            section.m_items.push_back(
                                static_cast<unsigned char>(
                                    ScanInt(static_cast<bool*>(0))));
                        }
                        out->m_rewardData.m_vec0.push_back(section);
                    }
                    break;
                }
                /* fall through */
            case 0x34ca:                // [select group]
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[select group]", 0x34ca));
                }
                else
                {
                    if (!ScanStr(&sub))
                    {
                        return false;   // ORIG 0x8916b3c ebx=0 → 返回
                    }
                    int job = getCharacterJob(sub.c_str());
                    int grade = ScanInt(static_cast<bool*>(0));
                    int packageKey = makeFairPVPEquipmentPackageKey(job, grade);

                    std::vector<std::vector<EventRewardItemInfo> > groups;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;      // ORIG 0x8916bb2 EOF → 主循环
                        }
                        if (token == "[package selection]")
                        {
                            std::vector<EventRewardItemInfo> items;  // 0x8921bee
                            int first;      // ORIG -0x1e0 无预清（ScanInt 直写）
                            // 注：ORIG 08916bed 条件在头（`call; xor $1; test; jne 退出`）；
                            // for(;;)+break 两形实测 +2 反劣（4.4 仍插前置 jmp）⇒ 维持 while。
                            while (ScanInt(&first))
                            {
                                EventRewardItemInfo info;   // ctor 0x891548a
                                info.m_field0 = first;
                                info.m_short8 =
                                    static_cast<short>(ScanInt(&ok));   // %ax
                                info.m_field4 = ScanInt(&ok);
                                ScanStr(&sub);   // 返回值被忽略（0x8916c45）
                                {
                                    std::string tmp(sub.c_str());
                                    std::vector<std::string> tokens;
                                    std::string delim(",");
                                    tokenizeStr(tmp, tokens, delim);
                                    for (int k = 0;
                                         k < (int)tokens.size(); ++k)
                                    {
                                        int growIdx = getGrowTypeIndex(
                                            tokens.at(k).c_str());
                                        info.m_growTypes.push_back(growIdx);
                                    }
                                }
                                items.push_back(info);
                            }
                            groups.push_back(items);
                        }
                        else if (token == "[/select group]")
                        {
                            // ORIG 0x8921ce9/0x8921d2e：make_pair(int&,vector&)
                            // + value_type 转换 ctor（两级临时析构链）。
                            out->m_rewardData.m_mapC.insert(
                                std::map<int,
                                    std::vector<std::vector<EventRewardItemInfo> >
                                >::value_type(
                                    std::make_pair(packageKey, groups)));
                            break;      // 回主循环
                        }
                        // 其余 token：ORIG 回内层头继续扫描
                    }
                    break;
                }
                /* fall through */
            case 0x34ff:                // [give group]
                if (!__inited)
                {
                    __lookup.insert(
                        DimSectionMap::value_type("[give group]", 0x34ff));
                }
                else
                {
                    if (!ScanStr(&sub))
                    {
                        return false;   // ORIG 0x891700c ebx=0 → 返回
                    }
                    int job = getCharacterJob(sub.c_str());
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;      // ORIG 0x891704d → 主循环
                        }
                        // ORIG 比较顺序：先 [grow type]（0x8917074），未中再查
                        // [/give group]（0x8917345）——operator== 多重集保持一致。
                        if (token == "[grow type]")
                        {
                            if (!ScanStr(&sub))
                            {
                                break;  // ORIG 0x891735a → 主循环
                            }
                            int growIdx = getGrowTypeIndex(sub.c_str());

                            std::vector<std::vector<EventRewardItemInfo> > groups;
                            for (;;)
                            {
                                if (!ScanType(token, true))
                                {
                                    break;  // ORIG 0x89170cf → ~groups 回 give 层
                                }
                                std::vector<EventRewardItemInfo> items;  // 0x8921bee
                                if (token == "[package selection]")
                                {
                                    // ORIG 0x891710f `mov %eax,-0x1c`：返回值落具名
                                    // 死局部（P0 槽往返 ⇒ 原码有该局部）。
                                    int dead20 = ScanInt(&ok);
                                    (void)dead20;
                                    EventRewardItemInfo info;  // ctor 0x891548a 一次
                                    while (ScanInt(&v))     // ORIG -0x1a8（复用）
                                    {
                                        info.m_field0 = v;
                                        info.m_short8 = static_cast<short>(
                                            ScanInt(&ok));               // %ax
                                        info.m_field4 = ScanInt(&ok);
                                        items.push_back(info);
                                        info.clear();                    // 0x89154bc
                                    }
                                    groups.push_back(items);
                                    items.clear();                       // 0x89171b4
                                }
                                else if (token == "[/grow type]")
                                {
                                    // ORIG 0x81b5c8e：key 对默认构造后手写字段，再
                                    // make_pair<std::pair<int,int>, vector>（0x8921e92
                                    // 按值实例化）+ 转换 ctor（0x8921ed6）。
                                    std::pair<int, int> key;
                                    key.first = job;
                                    key.second = growIdx;
                                    out->m_rewardData.m_map24.insert(
                                        std::map<std::pair<int, int>,
                                            std::vector<std::vector<EventRewardItemInfo> >
                                        >::value_type(
                                            std::make_pair<std::pair<int, int>,
                                                std::vector<
                                                    std::vector<EventRewardItemInfo> >
                                            >(key, groups)));
                                    groups.clear();                      // 0x89172c5
                                    break;  // 回 give 层
                                }
                                // 其余 token：继续内层（items 已构造无副作用）
                            }
                        }
                        else if (token == "[/give group]")
                        {
                            break;      // ORIG 0x8917352 → 主循环
                        }
                        // 其余 token：ORIG 0x891734c → give 层继续
                    }
                    break;
                }
            }
            // b28d-21: ORIG 0891735b `movzbl __inited; xor $1; test; je 回主循环`
            // ⇒ 原码负极性（未建表则 leave+置位+回 guard 头重派发）。
            if (!__inited)
            {
                __init_lock.leave();        // ORIG 0x8917369 建表收尾重派发
                __inited = true;
                continue;
            }
            break;                  // 派发完成 → 主循环下一 token
        }
    }
    return true;                        // ORIG 0x8917388 EOF → 1（尾部共享块）
}

// ============================================================================
// importEmotionTypeTagsScript —— ORIG T 0x0890de85
//   bool (const char*, stEmotionTypeTagsInfo*)
// [EMOTION INFO] 内：tag 入 m_tags(+0x00) 且记入 stOneEmotionData.m_0；
// 随后 4 值/3 布尔/字符串族填充元素后 push 入 m_data(+0x0c)；$END 结束
// m_18 字符串列表。任一 Scan 失败 → false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890de85。已修：load 检查
// 最前；函数级 token(-0x10)/unused(-0x14)/ok(-0x15)，out 判空在声明之后；`str`
// 改为 [EMOTION INFO] 块内局部（ORIG -0x1c）。p167 由 3 MISSING 转 CLEAR（29=29）。
// 残差 N86/N89（string 构造/析构族）+ N82 槽 ⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 245（gap −11），jcc 38/43
// （多 5 个）。逐位核对：3 处 `data.m_10/11/12 = (v==1)?1:0` 的 ORIG 形是
// `mov slot,%eax; cmp $1; sete %al`（无分支），4.4 恒折 `cmpl $1,slot; jne;
// mov $1; jmp; …`（每处 +1 jne +1 jmp）⇒ 属 P197④/setcc-fold 负例（4.1 槽比
// sete 不可复现，探针族已证），非语义差；其余 2 段为块首落点差。语义等价。
bool importEmotionTypeTagsScript(const char* path, stEmotionTypeTagsInfo* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    if (out == 0)
    {
        return false;                               // ORIG 0x890decb 判空
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[EMOTION INFO]")
        {
            stOneEmotionData data;                  // ctor 0x891b560 + clear
            std::string str;                        // ORIG -0x1c（块内构造）
            int tag = ScanInt(&ok);
            if (!ok) { return false; }
            out->m_tags.push_back(tag);
            data.m_0 = tag;

            data.m_4 = ScanInt(&ok);
            if (!ok) { return false; }
            if (!ScanStr(&data.m_8))
            {
                return false;
            }
            if (!ScanStr(&data.m_c))
            {
                return false;
            }
            int v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_10 = (v == 1) ? 1 : 0;           // sete
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_11 = (v == 1) ? 1 : 0;
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_12 = (v == 1) ? 1 : 0;

            if (!ScanStr(&data.m_14))
            {
                return false;
            }
            for (;;)
            {
                if (!ScanStr(&str))
                {
                    break;                          // 列表终止（含 $END 缺失情形）
                }
                if (str == "$END")
                {
                    break;
                }
                data.m_18.push_back(str);
            }
            out->m_data.push_back(data);
        }
    }
}

// ============================================================================
// importEquipmentGradeSampleInfo —— ORIG T 0x0890e18d
//   bool (const char*, stEquipmentGradeSample*)
// [grade_0]..[grade_4] 五节分别向五个 vector<unsigned int> push（push_backEOj
// 右值重载，值序一致）；out 判空在 load 之后（ORIG 0x890e1d3）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（190=190）— ORIG 0x0890e18d。load 检查最前，
// 随后 token(-0x34)/unused(-0x38，零引用)/ok(-0x39) 声明，再 out 判空；五节内层
// `for(;;){ v=ScanInt(&ok); if(!ok)break; push_back((unsigned int)v); }`
// （ORIG `xor $1; test; je body` 形；push_back 取右值重载 EOj，p167 CLEAR）。
bool importEquipmentGradeSampleInfo(const char* path,
                                    stEquipmentGradeSample* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x34
    std::string unused;                 // ORIG -0x38（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x39

    if (out == 0)
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[grade_0]")
        {
            for (;;)
            {
                int v0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[0].push_back((unsigned int)v0);
            }
        }
        else if (token == "[grade_1]")
        {
            for (;;)
            {
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[1].push_back((unsigned int)v1);
            }
        }
        else if (token == "[grade_2]")
        {
            for (;;)
            {
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[2].push_back((unsigned int)v2);
            }
        }
        else if (token == "[grade_3]")
        {
            for (;;)
            {
                int v3 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[3].push_back((unsigned int)v3);
            }
        }
        else if (token == "[grade_4]")
        {
            for (;;)
            {
                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[4].push_back((unsigned int)v4);
            }
        }
    }
    (void)unused;
    return true;
}
