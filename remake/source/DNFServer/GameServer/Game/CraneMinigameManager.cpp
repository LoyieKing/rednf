// df_game_r Game/ CraneMinigameManager 实现。
#include "CraneMinigameManager.h"

#include <string>

#include "NeedMaterialDataManager.h"
#include "GameTypes.h"          // cMyTrace
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat

// importActionMailScript —— ORIG T 0x0897f147（0x678 字节）。
// 完整邮件脚本解析依赖未建模类型，这里以可链接最小实现提供符号；
// 行为按推断：消费当前 token 后返回（外层解析器逐 token 推进，不会死循环）。
void importActionMailScript(Action_SendMail* mail)
{
    if (mail == 0)
        return;
    // 推断：完整实现待后续批次；此处仅清空收件人/物品状态。
    mail->m_user = 0;
    mail->m_itemIdx = 0;
    mail->m_count = 0;
}

// ============================================================================
// importCraneMinigameScript —— ORIG 0x088e3924（0x74d 字节）。
// 反汇编转写：loadRDARScriptFile("", path) 失败返回 false；随后逐 token 分派：
//   [viewCnt] → short@+0x00 = ScanInt
//   [item]    → new CraneItemInfo(0x14)；info->m_itemIdx=ScanInt；
//               info->m_id=(short)m_items.size()
//   [cnt]     → info->m_cnt(short)=ScanInt
//   [viewRatio] → info->m_viewRatio(float)=ScanFloat
//   [pickRatio] → info->m_pickRatio(int)=ScanInt
//   [/item]   → m_items.push_back(info)
//   [material] → +0x10 int=ScanInt；+0x14 short=ScanInt
//   [send mail] → importActionMailScript(+0x18)
//   [need material] → 循环 {pair.first=ScanInt; 失败则结束; pair.second=ScanInt;
//                       m_needMaterials.push_back}
// token 流耗尽返回 true（ORIG ebx=1）；未知 token 忽略。
// 关键字→处理映射与字段偏移均由 ORIG 跳转表/关键字表（0x8e0c980）实证；
// 以字符串比较等价转写（ORIG 的 map 查表为一次性初始化优化）。推断标注：
// 等价语义，非指令级一致。
// ============================================================================
bool importCraneMinigameScript(CraneMinigameScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    CraneItemInfo* currentItem = 0;
    while (ScanType(token, true))
    {
        if (token == "[viewCnt]")
        {
            bool ok = false;
            int value = ScanInt(&ok);
            script->m_viewCnt = static_cast<unsigned short>(value);
        }
        else if (token == "[item]")
        {
            bool ok = false;
            currentItem = new CraneItemInfo();
            currentItem->m_itemIdx = ScanInt(&ok);
            currentItem->m_id = static_cast<unsigned short>(
                script->m_items.size());
        }
        else if (token == "[cnt]")
        {
            bool ok = false;
            int value = ScanInt(&ok);
            if (currentItem != 0)
                currentItem->m_cnt = static_cast<unsigned short>(value);
        }
        else if (token == "[viewRatio]")
        {
            bool ok = false;
            float value = ScanFloat(&ok);
            if (currentItem != 0)
                currentItem->m_viewRatio = value;
        }
        else if (token == "[pickRatio]")
        {
            bool ok = false;
            int value = ScanInt(&ok);
            if (currentItem != 0)
                currentItem->m_pickRatio = value;
        }
        else if (token == "[/item]")
        {
            if (currentItem != 0)
                script->m_items.push_back(currentItem);
            currentItem = 0;
        }
        else if (token == "[material]")
        {
            bool ok = false;
            script->m_material = ScanInt(&ok);
            script->m_field14 = static_cast<short>(ScanInt(&ok));
        }
        else if (token == "[send mail]")
        {
            importActionMailScript(&script->m_sendMail);
        }
        else if (token == "[need material]")
        {
            while (true)
            {
                std::pair<int, int> material;
                bool ok = false;
                material.first = ScanInt(&ok);
                if (!ok)
                    break;
                material.second = ScanInt(&ok);
                script->m_needMaterials.push_back(material);
            }
        }
        // 其余 token 忽略（ORIG 默认分支）
    }
    return true;
}

CraneMinigameManager::CraneMinigameManager()
    : m_field10(0), m_sendMail(0), m_materials(), m_selectIdx(0)
{
}

CraneMinigameManager::~CraneMinigameManager() {}

// ORIG 0x080ebc6e：reset Action_SendMail 后导入脚本表；失败 trace 并返回 false。
bool CraneMinigameManager::init()
{
    Action_SendMail* mail =
        reinterpret_cast<Action_SendMail*>(
            reinterpret_cast<char*>(this) + 0x18);   // 内嵌对象直位复位
    mail->m_user = 0;
    mail->m_itemIdx = 0;
    mail->m_count = 0;

    if (!importCraneMinigameScript(
            reinterpret_cast<CraneMinigameScript*>(this),
            "Etc/craneMinigameItem.etc"))
    {
        cMyTrace trace("bool CraneMinigameManager::init()", 0x1d, 0);
        trace("Error : craneMinigameItem.etc load failed!!");
        return false;
    }
    return true;
}

// ORIG 0x080ebebc：m_selectIdx = count % m_materials.size()；
// 取对应 pair 调 NeedMaterialDataManager::setNeedMaterialInfo(m_field10, pair)。
void CraneMinigameManager::updateCraneItemNeedMaterial(int count)
{
    unsigned int size = m_materials.size();
    m_selectIdx = static_cast<unsigned char>(
        static_cast<unsigned int>(count) % size);
    const std::pair<int, int>& info = m_materials[m_selectIdx];
    NeedMaterialDataManager::getInstance()->setNeedMaterialInfo(m_field10, info);
}
