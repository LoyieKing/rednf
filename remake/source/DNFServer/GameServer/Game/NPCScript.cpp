#include "NPCScript.h"

#include <algorithm>

// ============================================================================
// CNPCScript —— 依据 ORIG 报告还原 5 个访问器/加载函数。
//   load / isKeyItem / isRewardLevel / getPeriodRewardType（CNPCScript/ 报告）
//   isExistRole（ORIG 0x86d2736 objdump）
// ============================================================================

// ---- 依赖：importNpcScript/NpcScript 本工程尚未还原 ----
// ORIG 中 CNPCScript 于 +0x04 内嵌一个 NpcScript 结构；load 调用
//   importNpcScript(NpcScript*, char const*) 解析脚本并回写首字。
//   NpcScript 结构体与 importNpcScript 均未在本工程重建，此处仅按 ORIG
//   ABI 声明（_Z15importNpcScriptP9NpcScriptPKc），实现在 NpcScript 子系统
//   还原时补齐。m_npcScriptData 占位区即代表该内嵌对象。 —「推断/依赖缺失」
struct NpcScript;
extern int importNpcScript(NpcScript*, char const*);

// importNpcScript（TSV 修复，ORIG 0x089fc264 T）。
// ORIG：loadRDARScriptFile(0x94fe680, path) 载入脚本 → 失败返回 0；
// 成功则 NpcScript::clear() 后用 ScanType/ScanInt 等从全局 RDAR 读取器逐字段
// 填充 +0x00 起的 NpcScript 数据，最后写回首字并返回 1。
// NpcScript 结构未还原（此处仅前置声明），RDA 逐字段 import 依赖该布局；
// 按链路优先提供签名匹配骨架：加载失败返回 0、成功占位返回 1（推断，不填充数据）。
int importNpcScript(NpcScript* npc, char const* path)
{
    (void)npc;
    // 推断：完整解析依赖 NpcScript 布局 + ScanType/ScanInt 序列；当前未能填充，
    // 保持调用方可链接。返回 0 表示加载未完成，避免他人误用未填充数据。
    (void)path;
    return 0;
}

namespace {

// isKeyItem 使用的比较器（ORIG 反编译中称 isSameItem）：
// 匹配 key item 表的 (key, itemId) 二元组。
struct NPC_IsSameItem
{
    const unsigned long m_key;
    const unsigned int  m_itemId;

    NPC_IsSameItem(unsigned long key, unsigned int id)
        : m_key(key), m_itemId(id) {}

    bool operator()(const std::pair<unsigned long, unsigned int>& item) const
    {
        return item.first == m_key && item.second == m_itemId;
    }
};

}  // namespace

// ORIG 0x85809a6:
//   ret = importNpcScript((NpcScript*)(this+4), path);
//   if (ret == 1) *(int*)this = *(int*)(this+4);
//   return ret == 1;
bool CNPCScript::load(char const* path)
{
    NpcScript* npc = reinterpret_cast<NpcScript*>(m_npcScriptData);
    int ret = importNpcScript(npc, path);
    if (ret == 1)
    {
        m_resultCopy = *reinterpret_cast<unsigned int*>(m_npcScriptData);
    }
    return ret == 1;
}

// ORIG 0x86d2736: 返回 m_roleMap.find(role) != m_roleMap.end()
//   （this+0x50 的 std::map<ENUM_NPC_ROLE,int>；ORIG 对角色 NONE=0 查询）。
bool CNPCScript::isExistRole(ENUM_NPC_ROLE role) const
{
    return m_roleMap.find(role) != m_roleMap.end();
}

// ORIG 0x8580fd0: 遍历 m_levelRewardList（stLevelRewardInfo），
//   命中 m_level == level 返回 1，否则 0。
bool CNPCScript::isRewardLevel(unsigned short level) const
{
    for (std::vector<CNPCScript_LevelRewardInfo>::const_iterator it =
             m_levelRewardList.begin();
         it != m_levelRewardList.end(); ++it)
    {
        if (it->m_level == level)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x8580e6a: std::find_if(m_keyItemList.begin(), end,
//                              isSameItem(key, id)) != end()
bool CNPCScript::isKeyItem(unsigned long key, unsigned int id) const
{
    return std::find_if(m_keyItemList.begin(), m_keyItemList.end(),
                        NPC_IsSameItem(key, id)) != m_keyItemList.end();
}

// ORIG 0x85811a4: m_periodRewardType == "item" -> 1；=="quest" -> 2；否则 0。
int CNPCScript::getPeriodRewardType() const
{
    if (m_periodRewardType == "item")
    {
        return 1;
    }
    if (m_periodRewardType == "quest")
    {
        return 2;
    }
    return 0;
}

// 构造/析构：成员均为 std 容器（自带默认构造/析构），空体即正确的
// ORIG 构造/析构语义（ORIG NpcScript ctor 的巨大初始化建于此嵌入结构上）。
CNPCScript::CNPCScript() {}
CNPCScript::~CNPCScript() {}