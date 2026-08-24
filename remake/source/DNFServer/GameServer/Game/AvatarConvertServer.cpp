// df_game_r Game/ AvatarConvertServer + ARAD::SCRIPT::AvatarConvert 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__AvatarConvert/*.md 与
// docs/class_func_reports/AvatarConvertServer/{AvatarConvertServer,~AvatarConvertServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "AvatarConvertServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证；std::map=0x18 与 ORIG 一致）----
typedef char avatar_convert_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert) == 0x18 ? 1 : -1];
typedef char avatar_convert_getinfo_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::GetInfo) == 0x8 ? 1 : -1];
typedef char avatar_convert_result_size_assert[sizeof(ARAD::SCRIPT::AvatarConvert::ConvertResult) == 0x24 ? 1 : -1];
typedef char avatar_convert_server_size_assert[sizeof(AvatarConvertServer) == 0x1c ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// GetInfo —— POD 聚合；trivial D1/D0 弱符号由编译器在 vector 销毁路径产生
// （ORIG W 0x088b3148）。
//
// ConvertResult —— ORIG W ctor 0x088b1f42 / dtor 0x088b1f8e。ORIG loadScript 先对
// 0x24 字节 memset(0) 再调用本 ctor，ctor 内仅默认构造两个 vector——标量不在此
// 清零，与 ORIG 一致。
// ============================================================================
AvatarConvert::ConvertResult::ConvertResult()
    : m_infos(), m_cumWeights()
{
}

AvatarConvert::ConvertResult::~ConvertResult()
{
}

// ============================================================================
// AvatarConvert —— ORIG ctor 0x088b1b0e：map ctor。dtor 0x088b1b22：仅 map dtor
// （不 delete 条目——释放须经 clearScript，ORIG 如此）。
// ============================================================================
AvatarConvert::AvatarConvert()
{
}

AvatarConvert::~AvatarConvert()
{
}

// ============================================================================
// loadScript —— ORIG T 0x88b1b36。"Etc/EmblemRenewal/" + "avatar_convert_jpn.etc"。
// 每条记录：TYPE 头 → grade=ScanInt、flag=(ScanInt==1) → 循环 ScanTypeOrInt 取
// 数值键 + 2×short（push GetInfo{key,idx,cnt}，累计 cnt 入 m_cumWeights）→
// 因 TYPE 退出时查重插入 m_convertMap（total=m_cumWeights.back()），因 EOF/非法
// 退出则整条丢弃。重复 grade：printf("%s duplicate grade:%d") 后返回 -2
// （ORIG 此路径不释放 cr——忠实保留）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
int AvatarConvert::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_convert_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    while (ScanType(token, true))
    {
        int totalWeight = 0;   // ORIG -0x40：每条记录重置
        // ORIG：operator new(0x24) → memset 0x24 字节 → ConvertResult::C1。
        ConvertResult* convertResult = new ConvertResult();
        convertResult->m_grade = ScanInt((bool*)0);
        convertResult->m_flag = (ScanInt((bool*)0) == 1);
        for (;;)
        {
            int key = ScanTypeOrInt(isType, token, isInt, true);
            if (isType || !isInt)
                break;   // TYPE → 插入分支；EOF/非法 → 丢弃该记录（由 isType 区分）
            GetInfo info;
            info.m_key = key;
            info.m_itemIndex = static_cast<short>(ScanInt((bool*)0));
            info.m_count = static_cast<short>(ScanInt((bool*)0));
            convertResult->m_infos.push_back(info);
            totalWeight += info.m_count;
            convertResult->m_cumWeights.push_back(totalWeight);
        }
        if (isType)
        {
            std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
            if (m_convertMap.find(convertResult->m_grade) != endIt)
            {
                printf("%s duplicate grade:%d", token.c_str(), convertResult->m_grade);
                return -2;
            }
            convertResult->m_totalWeight = convertResult->m_cumWeights.back();
            m_convertMap.insert(
                std::map<int, ConvertResult*>::value_type(convertResult->m_grade,
                                                          convertResult));
        }
    }
    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// clearScript —— ORIG T 0x88b1dc2：begin/end 各取一次，遍历 delete ConvertResult*
// （delete 表达式自带判空分支，与 ORIG 的 test→D1→operator delete 形态一致）
// → map.clear()。
// ============================================================================
void AvatarConvert::clearScript()
{
    std::map<int, ConvertResult*>::iterator it = m_convertMap.begin();
    std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
    for (; it != endIt; ++it)
        delete it->second;
    m_convertMap.clear();
}

// getConvertResult —— ORIG T 0x88b1e48：先默认构造迭代器再赋值 find 结果；
// 命中返回指针，未命中 0。
AvatarConvert::ConvertResult* AvatarConvert::getConvertResult(int grade)
{
    std::map<int, ConvertResult*>::iterator it;
    it = m_convertMap.find(grade);
    if (it != m_convertMap.end())
        return it->second;
    return 0;
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// AvatarConvertServer —— ORIG C1 0x081921b6 / D1 0x081921ec。基类口径同
// EmblemCompoundServer.cpp 注释（Singleton W 0x0819380a 未建模）。
// ============================================================================
AvatarConvertServer::AvatarConvertServer()
{
}

AvatarConvertServer::~AvatarConvertServer()
{
}

// Load —— ORIG T 0x081930ba：cMyTrace(pretty,0x37f,9,true,false) +
// operator()("\t- Loading Avatar Convert script - ") + loadScript()==0。
bool AvatarConvertServer::Load()
{
    cMyTrace trace("virtual bool AvatarConvertServer::Load()", 0x37f, 9, true, false);
    trace("\t- Loading Avatar Convert script - ");
    return loadScript() == 0;
}

// Destroy —— ORIG T 0x0819312c：return 1（基类槽1 void 约定）。
void AvatarConvertServer::Destroy()
{
}

// Test —— ORIG T 0x0819220c：空（非虚成员）。
void AvatarConvertServer::Test()
{
}
