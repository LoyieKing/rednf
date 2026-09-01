// ============================================================================
// df_game_r 还原 —— SkillShopGuideTextData 域（独占 TU）
//
// 报告：docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md（已完整阅读
// 汇编 + 反编译 C 两段）。
// 类方法（W 符号）布局证据：objdump 0x0891cb4c/0x08932804/0x089328e6；
//   分配尺寸 0x10（new_allocator::construct movl $0x10），C1/D1 仅 string 成员，
//   拷贝 ctor/op= 逐字段 int,int,string,byte,byte,byte。
// importSkillShopGuideText：ORIG T 0x08914483，无调用者（全量 objdump 无 call 命中），
//   语义：装载 SkillShopGuideText.etc，段键经 static map 查找（首次 SpinLock 保护
//   注册键值），"[character job]"+growType 为键、"[text]/[color]/[level range]"
//   填充数据向量后 insert 进 out map。
// ============================================================================

#include "SkillShopGuideTextData.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include "CEnvironment.h"    // class SpinLock / less_TCHAR
#include "LogManager.h"
#include "Quest.h"           // enum ENUM_CHARACTERJOB
#include "SyncScriptDeps.h"  // int getCharacterJob(const char*)

#include <map>
#include <vector>
#include <utility>

// ORIG T 0x088be63d（定义于 InitScriptProducers_6.cpp，本 TU 仅引用）
int getGrowTypeIndex(const char* name);

// ============================================================================
// SkillShopGuideTextData 四件套（ORIG W 0x0891cb4c / 0x0891cb62 /
// 0x08932804 / 0x089328e6）
// ============================================================================
SkillShopGuideTextData::SkillShopGuideTextData()
{
}

SkillShopGuideTextData::SkillShopGuideTextData(SkillShopGuideTextData const& o)
    : m_levelMin(o.m_levelMin), m_levelMax(o.m_levelMax), m_text(o.m_text),
      m_colorR(o.m_colorR), m_colorG(o.m_colorG), m_colorB(o.m_colorB)
{
}

SkillShopGuideTextData::~SkillShopGuideTextData()
{
}

SkillShopGuideTextData& SkillShopGuideTextData::operator=(SkillShopGuideTextData const& o)
{
    m_levelMin = o.m_levelMin;
    m_levelMax = o.m_levelMax;
    m_text = o.m_text;
    m_colorR = o.m_colorR;
    m_colorG = o.m_colorG;
    m_colorB = o.m_colorB;
    return *this;
}

// ============================================================================
// importSkillShopGuideText —— ORIG T 0x08914483
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md
// 返回 int：0=失败（文件加载失败/段内 ScanStr 失败），1=成功。
// 静态局部：__lookup（map<const char*,int,less_TCHAR>，惰性 guard+atexit）、
//   __init_lock（SpinLock，惰性 guard+atexit）、__inited（bool，SpinLock 保护）。
// 控制流（汇编 LAB_08914553 do 循环）：__inited==0 时持锁注册全部段键后置 1，
//   回 do 头以 find 路径重处理当前 token；__inited==1 直接 find 分派。
// ORIG 三个 static 的 guard 检查位于 do 头（08914553/089145cf/0891464b，每次
//   迭代经 guard 跳过初始化），故声明须在循环体内。
// 注册链按 ORIG fall-through：case 按数值升序，__inited==0 时逐段注册后落入
// 下一 case；段键注册用直接 pair 构造（ORIG pair<char_const(&)[N],int> 模板实例）。
// ============================================================================
int importSkillShopGuideText(
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> >,
             std::less<std::pair<ENUM_CHARACTERJOB, int> >,
             std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> > > > >& out)
{
    typedef std::map<std::pair<ENUM_CHARACTERJOB, int>,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> >,
                     std::less<std::pair<ENUM_CHARACTERJOB, int> >,
                     std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> > > > > OutMap;
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3318,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }

    std::pair<ENUM_CHARACTERJOB, int> key;
    key.first = (ENUM_CHARACTERJOB)0xb;
    key.second = -1;
    SkillShopGuideTextData data;
    std::vector<SkillShopGuideTextData> vec;
    std::string token;

    while (ScanType(token, true))
    {
        typedef std::map<const char*, int, less_TCHAR> LookupMap;
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int type;

    loop:
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(token.c_str());
            if (it != __lookup.end())
                type = it->second;
            else
                type = -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto loop;
            }
            type = 0;
        }

        switch (type)
        {
        case 0:
        case 0x332c:   // "[guide]"
            if (__inited) break;
            __lookup.insert(LookupMap::value_type("[guide]", 0x332c));
            // fall through
        case 0x3330:   // "[/guide]"
            if (__inited) break;
            __lookup.insert(LookupMap::value_type("[/guide]", 0x3330));
            // fall through
        case 0x3334:   // "[character job]"
            if (__inited)
            {
                std::string jobStr;
                bool ok = ScanStr(&jobStr);
                if (ok)
                {
                    key.first = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                    std::string growStr;
                    ok = ScanStr(&growStr);
                    if (ok)
                        key.second = getGrowTypeIndex(growStr.c_str());
                }
                if (!ok)
                    return 0;
                break;
            }
            __lookup.insert(LookupMap::value_type("[character job]", 0x3334));
            // fall through
        case 0x3342:   // "[/character job]"
            if (__inited)
            {
                // ORIG 0x08914a7b：直接就地构造 out 的 value_type
                // （_ZNSt4pairIKS_I17ENUM_CHARACTERJOBiESt6vectorI22SkillShopGuideTextData
                //   SaIS4_EEEC1IRS1_RS6_EEOT_OT0_ = pair<const pair<JOB,int>,vector>
                //   的 pair<pair<JOB,int>&, vector&> 模板构造），随后 map::insert；
                // make_pair 会多出 pair<pair<JOB,int>,vector> → value_type 的转换临时。
                out.insert(OutMap::value_type(key, vec));
                key.first = (ENUM_CHARACTERJOB)0xb;
                key.second = -1;
                vec.clear();
                break;
            }
            __lookup.insert(LookupMap::value_type("[/character job]", 0x3342));
            // fall through
        case 0x334c:   // "[text info]"
            if (__inited) break;
            __lookup.insert(LookupMap::value_type("[text info]", 0x334c));
            // fall through
        case 0x3350:   // "[/text info]"
            if (__inited) break;
            __lookup.insert(LookupMap::value_type("[/text info]", 0x3350));
            // fall through
        case 0x3355:   // "[text]"
            if (__inited)
            {
                ScanStr(&data.m_text);
                break;
            }
            __lookup.insert(LookupMap::value_type("[text]", 0x3355));
            // fall through
        case 0x335a:   // "[color]"
            if (__inited)
            {
                data.m_colorR = (unsigned char)ScanInt((bool*)0);
                data.m_colorG = (unsigned char)ScanInt((bool*)0);
                data.m_colorB = (unsigned char)ScanInt((bool*)0);
                break;
            }
            __lookup.insert(LookupMap::value_type("[color]", 0x335a));
            // fall through
        case 0x3361:   // "[level range]"
            if (__inited)
            {
                data.m_levelMin = ScanInt((bool*)0);
                data.m_levelMax = ScanInt((bool*)0);
                break;
            }
            __lookup.insert(LookupMap::value_type("[level range]", 0x3361));
            break;
        default:
            break;
        }

        if (__inited)
            continue;
        __init_lock.leave();
        __inited = true;
        goto loop;
    }

    return 1;
}
// ============================================================================
// importSkillShopTree —— ORIG T 0x0891427f（范围 [0x0891427f, 0x08914482]）
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopTree.md
// 返回 int：0=文件加载失败，1=成功。与 importSkillShopGuideText 同属
//   RDARScriptEtcParameter.cpp 日志域；装载 SkillShopTree.etc 后 clear out，
//   逐 token："[skill tree]" 段内循环 ScanStr 职业名 → getCharacterJob 转枚举、
//   ScanStr 树名，pair 构造后 insert 进 out map；ScanStr 失败跳出内层循环回到
//   外层 do；遇到 "[/skill tree]" 或 EOF 结束返回 1。
// ============================================================================
int importSkillShopTree(
    const char* path,
    std::map<ENUM_CHARACTERJOB, std::string, std::less<ENUM_CHARACTERJOB>,
             std::allocator<std::pair<ENUM_CHARACTERJOB const, std::string> > >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x32eb,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }
    out.clear();
    std::string token;
    std::string jobStr;
    std::string treeStr;
    while (ScanType(token, true))
    {
        if (token == "[skill tree]")
        {
            while (ScanStr(&jobStr))
            {
                ENUM_CHARACTERJOB job = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                if (!ScanStr(&treeStr))
                    break;
                // ORIG 0x0891439f：直接就地构造 value_type
                // （_ZNSt4pairIK17ENUM_CHARACTERJOBSsEC1IRS0_RSsEEOT_OT0_ =
                //   pair<const ENUM_CHARACTERJOB,string> 的 pair<JOB&,string&> 构造）
                // 后 map::insert(value_type const&)；make_pair 会多出转换临时。
                out.insert(
                    std::map<ENUM_CHARACTERJOB, std::string,
                             std::less<ENUM_CHARACTERJOB>,
                             std::allocator<std::pair<ENUM_CHARACTERJOB const,
                             std::string> > >::value_type(job, treeStr));
            }
        }
        else if (token == "[/skill tree]")
        {
            break;
        }
    }
    return 1;
}

// ============================================================================
// getSkillMaxLevelOnSteal —— ORIG T 0x08a9af4f（0x53 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillMaxLevelOnSteal.md
// 与 getSkillSpendSPOnSteal（SkillSlot.cpp）共用 .rodata 常量 0x8e2a180 = 100.0f：
//   f = penalty / 100.0f；r = (int)(sp * f)（fistpl 控制字改 0x0c → RC=11 截断
//   向零，与 C 浮点转整一致）；返回 std::max(1, r)。
// ============================================================================
int getSkillMaxLevelOnSteal(int sp, int penalty)
{
    float f = (float)penalty / 100.0f;
    int r = (int)((float)sp * f);
    return std::max(1, r);
}

// ============================================================================
// getSkillNeedLevelOnSteal —— ORIG T 0x08a9aff5（0xe 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillNeedLevelOnSteal.md
// 汇编仅 lea (%edx,%eax,1) + ret：直接返回 param_1 + param_2。
// ============================================================================
int getSkillNeedLevelOnSteal(int level, int add)
{
    return level + add;
}
