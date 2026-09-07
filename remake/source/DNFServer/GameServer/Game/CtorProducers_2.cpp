// ============================================================================
// CtorProducers_2.cpp — Ctor_Group2 批次：构造/装载 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写）：
//   DisjointScript         C1 0x08375d4c / clear 0x08375bca / D1 0x0837dff8
//   DisJoint               C1 0x0837e532 / LoadDisJoint T 0x0847300c
//   StrikerScript          getInsance/ctor/dtor/clear/create/destory（instance_
//                          B 0x09500978）/ importScript 系列声明见头，定义待续批
//   CharacManageScript     instance_ B 0x094f6b98 / getInsance/ctor/dtor/clear/importScript
//   SDCActiveStatusSyncCommon ctor W 0x08234870 / dtor W 0x08234876（空体）
//   SDCActiveStatusSyncServer ctor W 0x0823487c / dtor W 0x08234890 /
//                          loadActiveStatusScript T 0x0808e13c
//
// 不在本 TU 提供（避免链接重复，归属既有注册 TU）：
//   LotteryInfo C1/clear/D1 → CStackableItem.cpp 已定义（ORIG 0x083706d4/069c/4cfc）。
//
// 声明先行（定义待对应域批次，见交付报告）：
//   importDisjointScript / importCompoundAvatarTable / importActiveStatusTable /
//   ContentsType::getType / importSkillDataUp
#include "DisJoint.h"
#include "StrikerScript.h"
#include "NpcScript.h"          // STScriptFileList 完整定义（create 用）
#include "CharacManageScript.h"
#include "CItemUpgrade_Separate.h"
#include "STStackableScript.h"        // ENUM_MODULE_TYPE（DataManagerInitDepends 前置要求）
#include "DataManagerInitDepends.h"   // SDCActiveStatusSyncServer / WongWork::CCompoundAvatar
#include "PvPTypes.h"                 // CMTRand
#include "DNFLexWrapper.h"            // ScanType/ScanInt/ScanFloat/ScanStr/loadRDARScriptFile

#include <cstdio>
#include <ctime>
#include <new>
#include <string>
#include <vector>

// ============================ DisjointScript ============================


// ---- DisjointSkillScript（ORIG W 0x084735fe/0x084735d2；布局 4×int）----
DisjointSkillScript::DisjointSkillScript()
{
    // ORIG 0x084735fe：仅调用 clear()。
    clear();
}

void DisjointSkillScript::clear()
{
    // ORIG 0x084735d2：{0xb, 0, 0, 4} 四连写。
    // 定义置于 ctor 之后：4.4 对"被调函数本 TU 未定义"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x084735fe 帧形一致（探针 /tmp/near7/p7-p8）。
    m_field00 = 0xb;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 4;
}

// ---- DisjointScript::._381（命名类 _381；成员 +0x00 int m_count / +0x04 vector<int> m_items）----
// ORIG ctor _ZN14DisjointScript5._381C1Ev t 0x08348088 (0x16)：仅构造 +4 vector<int>；
// dtor _ZN14DisjointScript5._381D1Ev t 0x0834809e：逆序析构 vector。
// GCC 对 ctor/dtor 的 asm 别名无效（硬限制），故自然名 `_381`(4_381) 处重建原 ctor/dtor，
// 末尾 `.set` 汇编别名强制 ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。布局不变。
DisjointScript::_381::_381()
{
    // ORIG 0x08348088：仅默认构造 members（vector 初值由自动构造完成）。
}

DisjointScript::_381::~_381()
{
    // ORIG 0x0834809e：逆序析构各成员（vector 自动析构）。
}

DisjointScript::DisjointScript()
{
    // ORIG 0x08375d4c：成员容器自动构造（含 6×匿名 _381@+0x5c8 循环调其 C1），
    // 函数体仅调 clear()；m_skillSelect/m_fieldCC/m_creationConst 的初值由
    // clear()（0x08375bca）写入，ctor 不再重复赋值（ORIG 实证无这些 store）。
    clear();
}

void DisjointScript::clear()
{
    // ORIG 0x08375bca：5 容器 clear → 4 LotteryInfo::clear → +0xbc=-1 →
    // skills clear → (+0xcc,+0xdc) 清零 → 三重循环清零 m_grid。
    for (int i = 0; i < 5; ++i)
    {
        m_cubeIndexVecs[i].clear();
    }
    for (int i = 0; i < 4; ++i)
    {
        m_legacyInfo[i].clear();
    }
    m_skillSelect = -1;
    m_skills.clear();
    // ORIG 0x08375bca 续：i=0..1 对 (+0xcc,+0xd0) 两个 int 位清零（即两个 float），
    // 随后三重循环 a(job)=0..10、b(grade)=0..3、c(slot)=0..4 清零
    // m_avatarDisjoint[a][b][c]（索引式 (c+b*5+a*20+0x34)，字节区 +0xe0..+0x44f）。
    m_fieldCC = 0;                                   // +0xcc
    m_skillProbD0 = 0.0f;                            // +0xd0
    for (int a = 0; a < 11; ++a)
    {
        for (int b = 0; b < 4; ++b)
        {
            for (int c = 0; c < 5; ++c)
            {
                m_avatarDisjoint[a][b][c] = 0;
            }
        }
    }
}

DisjointScript::~DisjointScript()
{
    // ORIG 0x0837dff8：逆序析构各容器成员。
}

// ============================ DisJoint ============================

DisJoint::DisJoint()
{
    // ORIG 0x0837e532：基类构造 + 6×vector<int> @+0x628 默认构造。
}

DisJoint::~DisJoint()
{
    // ORIG 0x0837dff8 链：基类析构 + 6 个结果 vector 析构。
}

bool DisJoint::LoadDisJoint()
{
    // ORIG 0x0847300c：import 成功则把 [cube index] 第 5 组首值与其
    // [additional result] 各组元素聚合到 +0x628 起的 6 个 vector。
    if (!importDisjointScript(this, "Etc/Disjoint.etc"))
    {
        std::puts("DisJoint Script Error: Etc/Disjoint.etc");
        return false;
    }

    for (int i = 0; i < 6; ++i)
    {
        int value = m_cubeIndexVecs[4].at(0);        // at(this+0x30, 0)；ORIG 显式 _ZNSt6vectorIiSaIiEE2atEj
        m_resultVecs[i].push_back(value);
        const int count = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(&m_additional[0]) + i * 0x10);
        for (int j = 0; j < count; ++j)
        {
            m_resultVecs[i].push_back(m_additional[i].m_items[j]);
        }
    }
    return true;
}

// ============================ StrikerScript ============================


// ORIG 静态成员单例指针（nm B 数据；getInsance 懒加载落点）。
StrikerScript* StrikerScript::instance_ = 0;              // _ZN13StrikerScript9instance_E B 0x09500978
CharacManageScript* CharacManageScript::instance_ = 0;    // _ZN18CharacManageScript9instance_E B 0x094f6b98


StrikerScript* StrikerScript::getInsance()
{
    // ORIG 0x08a9d836：懒加载单例（new 0xb4 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new StrikerScript;
    }
    return instance_;
}

void StrikerScript::clear()
{
    // ORIG 0x08a9dbb4：标量复位 + 容器 clear。
    m_tagCostTime = 0;
    m_tagCharacterSetting = 0;
    m_strikerCombo = 0;
    m_strikerStartTime = 0;
    m_strikerEndTime = 0x1f4;                    // 500
    m_moduleImpossible.clear();
    m_dungeonImpossible.clear();
    m_field38 = 0;
    m_field3c = 0;
    m_fileList = 0;                              // +0x90
    m_loaded = 0;                                // +0xa0 byte
    m_field64 = 0;                               // +0x64
    m_field68 = 0;                               // +0x68
    m_commonCoolTime = 0;                        // +0xa4
    m_positionType = 0;                          // +0xac
    m_upgradeRevision = 0x63;                    // +0xb0 = 99
}

void StrikerScript::create()
{
    // ORIG 0x08a9dc70：new STScriptFileList(0x18) → +0x90。
    m_fileList = new STScriptFileList();
}

void StrikerScript::destory()
{
    // ORIG 0x08a9dcbe：delete 文件列表（STScriptFileList::D1 + operator delete）；
    // 遍历技能 map delete 各 vector 并置空；尾部 m_skillMap.clear()。
    delete reinterpret_cast<STScriptFileList*>(m_fileList);
    m_fileList = 0;
    for (std::map<int, std::vector<StrikerSkill>*>::iterator it = m_skillMap.begin();
         it != m_skillMap.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
            it->second = 0;
        }
    }
    m_skillMap.clear();   // ORIG 尾部 map clear
}



StrikerScript::StrikerScript()
{
    // ORIG 0x08a9d88e：各容器默认构造（成员自动）→ clear() → create()。
    clear();
    create();
}

StrikerScript::~StrikerScript()
{
    // ORIG 0x08a9da28：destory() 后逆序析构容器成员。
    destory();
}

// ============================ CharacManageScript ============================

CharacManageScript* CharacManageScript::getInsance()
{
    // ORIG 0x088e0674：懒加载单例（new 0x88 → ctor → 存 instance_）。
    if (instance_ == 0)
    {
        instance_ = new CharacManageScript;
    }
    return instance_;
}

CharacManageScript::CharacManageScript()
{
    // ORIG 0x088e06cc：vector<ManagePointBonus>@+0、vector<int>@+0xc、
    // map<int,int>@+0x18/+0x30、map<int,string>@+0x4c、map<int,int>@+0x64、
    // vector<ContentsOptionRevision>@+0x7c 默认构造（成员自动）后调 clear()。
    clear();
}

void CharacManageScript::clear()
{
    // ORIG 0x088e0904：两 vector clear → +0x18/+0x30 map clear →
    // +0x48=0 → map<int,string> clear → +0x64 map clear（依 asm 顺序）。
    m_pointBonus.clear();
    m_manageLevelPoint.clear();
    m_expBonus.clear();
    m_goldBonus.clear();
    m_manageLevelMax = 0;
    m_basicExplain.clear();
    m_manageOption.clear();
}

CharacManageScript::~CharacManageScript()
{
    // ORIG 0x088e07f6：逆序析构 +0x7c vector → 4×map → +0xc vector → +0x00
    // vector<ManagePointBonus>（成员自动，顺序一致）。
}

bool CharacManageScript::importScript(const char* path)
{
    // ORIG 0x088e0968：RDAR 装载失败 ret0；分节扫描 [point bonus]/
    // [manage level point]/[manage level max]/[exp bonus]/[gold bonus]/
    // [manage basic explain]/[manage option]/默认 ContentsOptionRevision 行。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string value;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[point bonus]")
        {
            while (true)
            {
                ManagePointBonus bonus;              // ORIG 栈 ctor 0x088e12f6
                bonus.m_field00 = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 回 ScanType 主循环（ORIG jmp +0x46）
                }
                bonus.m_field04 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                bonus.m_field08 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                m_pointBonus.push_back(bonus);
            }
        }
        else if (token == "[manage level point]")
        {
            int level = 0;
            while ((level = ScanInt(&ok)), ok)       // do-while 形态（asm +0x11a）
            {
                m_manageLevelPoint.push_back(level);
            }
        }
        else if (token == "[manage level max]")
        {
            m_manageLevelMax = ScanInt(&ok);         // +0x48
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[exp bonus]" || token == "[gold bonus]")
        {
            std::map<int, int>& table =
                (token == "[exp bonus]") ? m_expBonus : m_goldBonus;   // +0x18 / +0x30
            while (true)
            {
                const int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                const int val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                const std::pair<std::map<int, int>::iterator, bool> ins =
                    table.insert(std::make_pair(key, val));
                if (!ins.second)                     // insert.second==0 → 查重告警
                {
                    std::printf("ERROR [%s][%s] key duplicated\n",
                                path, token.c_str());
                }
            }
        }
        else if (token == "[manage basic explain]")
        {
            m_basicExplain.clear();                  // 先 clear 再按行号写入
            int index = 0;
            while (ScanStr(&value))
            {
                m_basicExplain[index] = value;
                ++index;
            }
        }
        else if (token == "[manage option]")
        {
            m_manageOption.clear();
            while (true)
            {
                const int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                m_manageOption[key] = ScanInt((bool*)0);   // ORIG ScanIntPb(NULL)
            }
        }
        else
        {
            m_optionRevision.clear();
            while (ScanStr(&value))
            {
                ContentsOptionRevision rev;          // POD {int type; int rev;}
                rev.m_type = ContentsType::getType(value.c_str());
                rev.m_field04 = ScanInt((bool*)0);
                m_optionRevision.push_back(rev);
            }
        }
    }
    return true;                                     // ScanType 耗尽 ret1
}

// ============================ SDCActiveStatusSync Common/Server ============================

SDCActiveStatusSyncCommon::SDCActiveStatusSyncCommon()
{
    // ORIG W 0x08234870：空体 ret。
}

SDCActiveStatusSyncCommon::~SDCActiveStatusSyncCommon()
{
    // ORIG W 0x08234876：空体 ret。
}

bool SDCActiveStatusSyncServer::loadActiveStatusScript()
{
    // ORIG T 0x0808e13c：尾透传 return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc")。
    return importActiveStatusTable("Etc/ActiveStatusTable_ex.etc");
}

// ============================ DisjointScript::._381 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；对匿名嵌套 struct 又按
// 本 TU/行号计数命名（如 4._44），与 ORIG `5._381` 不一致。故在 DisjointScript 类内
// 命名 `_381`，用 `.set` 把自然 mangled `4_381*` 强制改名为 ORIG 带点号 `5._381*`。
// ORIG 为局部 `t` 符号：命名类成员默认 W/全局 weak，binding 不敏感（compare 按符号名匹配），
// 以 nm 存在为准。cf. docs/class_func_reports/DisJoint 匿名 ._381（0x08348088/0x0834809e）。
asm(".set _ZN14DisjointScript5._381C1Ev, _ZN14DisjointScript4_381C1Ev");
asm(".set _ZN14DisjointScript5._381C2Ev, _ZN14DisjointScript4_381C2Ev");
asm(".set _ZN14DisjointScript5._381D1Ev, _ZN14DisjointScript4_381D1Ev");
asm(".set _ZN14DisjointScript5._381D2Ev, _ZN14DisjointScript4_381D2Ev");
