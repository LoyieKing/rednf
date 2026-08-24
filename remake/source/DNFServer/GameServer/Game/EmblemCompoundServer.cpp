// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__EmblemCompound/*.md 与
// docs/class_func_reports/EmblemCompoundServer/{EmblemCompoundServer,~EmblemCompoundServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "EmblemCompoundServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证）----
typedef char emblem_compound_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound) == 0x84 ? 1 : -1];
typedef char emblem_compound_result_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound::Result) == 0xa ? 1 : -1];
typedef char emblem_compound_server_size_assert[sizeof(EmblemCompoundServer) == 0x88 ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// EmblemCompound —— ORIG ctor 0x088b1408 / dtor 0x088b14c2。
// 成员按声明序自构造/自析构即与 ORIG 一致：
//   ctor：map(+0x00) → map(+0x18) → 7×vector(+0x30 起，步长 0xc)
//   dtor：逆序析构 7 个 vector → map(+0x18) → map(+0x00)
// ============================================================================
EmblemCompound::EmblemCompound()
{
}

EmblemCompound::~EmblemCompound()
{
}

// ============================================================================
// loadScript —— ORIG T 0x088b154a。
// "Etc/EmblemRenewal/" + "emblem_compound_jpn.etc"；三节：
//   [emblem compound cost]   grade value         → m_cost（重复 grade → printf+-2）
//   [emblem compound result] grade 4×short flag  → m_result（new Result(0xa)；
//                              重复 grade → delete+printf+-2）
//   [emblem list]            listIdx itemIndex   → m_list[listIdx].push_back
// 循环形态与 ORIG 相同：节内由 ScanTypeOrInt 取数值项（isType==false&&isInt==true
// 才处理）；遇 TYPE/EOF 退出后回到外层 ScanType(token,true)。查重均为
// end()→find()→operator==、命中则插入否则报错的分支形态（与 ORIG 一致）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
int EmblemCompound::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "emblem_compound_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    while (ScanType(token, true))
    {
        if (token == "[emblem compound cost]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                int value = ScanInt((bool*)0);   // ORIG +0xb6：value 先扫再查重
                std::map<int, int>::iterator endIt = m_cost.end();
                if (m_cost.find(grade) == endIt)
                    m_cost.insert(std::map<int, int>::value_type(grade, value));
                else
                {
                    printf("[emblem compound cost] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem compound result]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG：operator new(0xa) 裸分配 POD，字段直接写入（无构造调用）。
                Result* result = static_cast<Result*>(::operator new(sizeof(Result)));
                result->m_listIdx = static_cast<short>(ScanInt((bool*)0));
                result->m_itemIndex = static_cast<short>(ScanInt((bool*)0));
                result->m_itemCount = static_cast<short>(ScanInt((bool*)0));
                result->m_field6 = static_cast<short>(ScanInt((bool*)0));
                result->m_selectable = (ScanInt((bool*)0) == 1);
                std::map<int, Result*>::iterator endIt = m_result.end();
                if (m_result.find(grade) == endIt)
                    m_result.insert(std::map<int, Result*>::value_type(grade, result));
                else
                {
                    ::operator delete(result);
                    printf("[emblem compound result] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem list]")
        {
            for (;;)
            {
                int listIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                unsigned long itemIndex = static_cast<unsigned long>(ScanInt((bool*)0));
                // ORIG 直接 this + listIdx*12 + 0x30 索引（无边界检查，忠实保留）。
                m_list[listIdx].push_back(itemIndex);
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// clearScript —— ORIG T 0x088b1926：清 cost map → begin/end 各取一次后遍历
// delete Result* → 清 result map。不清理 m_list（ORIG 如此）。
// ============================================================================
void EmblemCompound::clearScript()
{
    m_cost.clear();
    std::map<int, Result*>::iterator it = m_result.begin();
    std::map<int, Result*>::iterator endIt = m_result.end();
    for (; it != endIt; ++it)
        ::operator delete(it->second);
    m_result.clear();
}

// getCost —— ORIG T 0x088b19b0：find→end→operator!=，命中返回 value，未命中 -1。
int EmblemCompound::getCost(int grade)
{
    std::map<int, int>::iterator it = m_cost.find(grade);
    if (it != m_cost.end())
        return it->second;
    return -1;
}

// getResult —— ORIG T 0x88b1a14：同上形态，命中返回 Result*，未命中 0。
EmblemCompound::Result* EmblemCompound::getResult(int grade)
{
    std::map<int, Result*>::iterator it = m_result.find(grade);
    if (it != m_result.end())
        return it->second;
    return 0;
}

std::vector<unsigned long>* EmblemCompound::getList(int grade)
{
    // ORIG：先默认构造迭代器，再赋值 find 结果（帧内多出 C1 调用与拷贝）。
    std::map<int, Result*>::iterator it;
    it = m_result.find(grade);
    if (it != m_result.end())
        return &m_list[it->second->m_listIdx];
    return 0;
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// 注：ORIG C1 中间的 ARAD::Singleton<EmblemCompoundServer>::Singleton()（W
// 0x08193636，空基类构造）未建模——与 CerashopAddRestrict::Manager 同口径，
// Singleton<T> 采用 Arad_DataManager.h 的 Meyers 模板。
// ============================================================================
EmblemCompoundServer::EmblemCompoundServer()
{
}

EmblemCompoundServer::~EmblemCompoundServer()
{
}

// Load —— ORIG T 0x081915ba：cMyTrace(pretty,0x1c7,9,true,false) +
// operator()("\t- Loading Emblem Compound script - ") + loadScript()==0。
bool EmblemCompoundServer::Load()
{
    cMyTrace trace("virtual bool EmblemCompoundServer::Load()", 0x1c7, 9, true, false);
    trace("\t- Loading Emblem Compound script - ");
    return loadScript() == 0;
}

// Destroy —— ORIG T 0x0819162c：return 1。基类 ScriptInterface 槽1 为 void
// （仓库权威约定），可观察行为一致。
void EmblemCompoundServer::Destroy()
{
}

// Test —— ORIG T 0x081915b4：空（非虚成员）。
void EmblemCompoundServer::Test()
{
}
