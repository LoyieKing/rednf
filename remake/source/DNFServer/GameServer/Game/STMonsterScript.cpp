// ============================================================================
// df_game_r 还原 —— STMonsterScript 类方法（G5 怪物脚本表）
// ============================================================================

#include "STMonsterScript.h"

#include <stdio.h>
#include <string.h>

#include "DNFLexWrapper.h"

// ---- 类态方法（ORIG ctor 0x8368e14 / dtor 0x8378dac / op= 0x8377994 /
//      Clear 0x08a398aa）。
// ORIG STMonsterScript 为 ~0x1d24 字节大对象（string/vector/多容器，偏移与镜像
// 完全不同）；本头是 ABI 安全的最小语义镜像（m_field0/m_str4/m_vec7c）。
// 故以下方法按 ORIG 对各镜像成员的语义还原，其余字段构造/释放/拷贝依赖
// MonsterSemanticLayout 全布局建模（「推断」-镜像范围达成现状语义一致）。

STMonsterScript::STMonsterScript()
{
    // ORIG ctor 先构造各 string/vector 成员再调用 Clear()（报告 C 代码一致）。
    // 镜像的 string/vector 由编译器默认构造，显式 Clear() 完成成员初始化语义。
    Clear();
}

STMonsterScript::~STMonsterScript()
{
    // ORIG dtor 依序释放各容器成员。镜像成员为自动存储的 string/vector，
    // 析构时编译器自动释放，空体即等价语义（「推断」）。
}

STMonsterScript& STMonsterScript::operator=(const STMonsterScript& other)
{
    // ORIG op=(0x8377994) 逐成员深拷贝（string 用 Ss::operator=、vector 用
    // vector::operator=）。对镜像成员逐个深拷贝即完整等价。
    if (this != &other)
    {
        m_field0 = other.m_field0;
        m_str4 = other.m_str4;
        m_vec7c = other.m_vec7c;
    }
    return *this;
}

void STMonsterScript::Clear()
{
    // ORIG Clear(0x08a398aa) 复位全部成员；对镜像成员 +0(m_field0)/+4(m_str4)/
    // +0x7c(m_vec7c)，报告汇编分别执行清零/置空字符串/清空 vector。
    m_field0 = 0;
    m_str4.clear();
    m_vec7c.clear();
}

int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    // EDR 脚本解析器。ORIG 0x8a12827（13841 insn）流程：
    //   loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path) →
    //   Clear() 复位 → 逐行 ScanType(tag) + ScanInt(列) → 依模块标签分派写入各成员。
    // 本镜像仅建模 m_field0(+0)/m_str4(+4)/m_vec7c(+0x7c)；
    // 完整字段表依赖 STMonsterScript 全布局（属 MonsterSemanticLayout 范围），
    // 故此处还原真实 EDR/Scan 骨架，仅把可镜像容纳的成员写入，其余标注「推断」。
    (void)type;
    (void)flag;

    // ORIG 目录全局 g_szMonsterScriptBaseDirectory 未在重建源定义，
    // 以等价字面量 "Script/MonsterScript" 代替（「推断」）。
    if (!loadRDARScriptFile("Script/MonsterScript", path))
    {
        return 0;
    }
    script->Clear();

    std::string tag;
    int value = 0;

    // 头部：模块标签行。ORIG 首先对标签行 processModuleTagSet 分派。
    if (!ScanType(tag, true))
    {
        return 0;
    }

    // 数值 id → m_field0（ORIG 首列即脚本/怪物索引；「推断」：镜像首成员即该索引）。
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_field0 = value;

    // 能力/系数行：ScanType(label) + 若干整数列 → m_vec7c（镜像能力参数向量，
    // 与 upgradeAbilityByMonsterParameterCategory（STMonsterScript.h）入参对应）。
    // 「推断」：ORIG 各系数列依 tag 分派到众多成员，镜像仅能以 m_vec7c 承接。
    while (ScanType(tag, true))
    {
        if (!ScanInt(&value))
        {
            break;
        }
        script->m_vec7c.push_back(value);
    }

    return 1;
}

bool ImportMonsterBaseTable(STMonsterScript* script, const char* path)
{
    // ORIG 0x8a3c736 是独立表解析：loadRDARScriptFile(base,path) 后以
    // ScanInt(bool*) 循环填 vector<MonsterBaseParameter>(+0x12c)，
    // 与 ImportMonsterScript 不同。镜像未建模 +0x12c 向量，
    // 故按既有策略委托 ImportMonsterScript 只承接 m_field0（「推断」）。
    return ImportMonsterScript(script, 0, path, false) != 0;
}


bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    (void)extra;
    FILE* file = fopen(path, "w");
    if (!file)
    {
        return false;
    }
    fprintf(file, "%d\n%s\n", script->m_field0, script->m_str4.c_str());
    fclose(file);
    return true;
}

int getMonsterTableIndex(const STMonsterScript* script, int a, int b,
                         bool c, bool d)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    return script ? script->m_field0 : -1;
}

void setBasicAbilityByLevel(STMonsterScript* script, ActiveStaticInfo* info,
                            unsigned int level, int dungeonIdx, int roleType,
                            bool a, bool b)
{
    (void)script;
    (void)info;
    (void)level;
    (void)dungeonIdx;
    (void)roleType;
    (void)a;
    (void)b;
}

int getRandomChampionElement(STMonsterScript* script, int seed)
{
    (void)script;
    return (seed & 0x7fffffff) % 4;
}
