// df_game_r Game/ CraneMinigameManager 实现。
#include "CraneMinigameManager.h"

#include <string>

#include "NeedMaterialDataManager.h"
#include "PacketGuard.h"
#include "CUser.h"             // Send(PacketGuard&)
#include "GameTypes.h"          // cMyTrace
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat

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

// ============================================================================
// CraneMinigameScript C1/D1 —— ORIG 0x088e3736 / 0x088e37a8（外联定义，
// 产独立符号；头内不再内联）。
// C1：空体，仅按声明序默认构造 m_items(+0x04) → m_sendMail(+0x18) →
// m_needMaterials(+0x5c)；m_viewCnt/m_material/m_field14 故意不初始化（ORIG 无 store）。
// D1：begin/end 迭代器（声明序 it 先、end 后 = ORIG 栈槽 -0xc/-0x10），
// 循环内 O0 三次独立 operator*（判空 / delete / 置 0，P48 无 CSE），
// clear 后编译器按声明逆序析构 m_needMaterials → m_sendMail → m_items（ORIG 同序）。
// ============================================================================
CraneMinigameScript::CraneMinigameScript()
{
}

// ORIG W 0x088e3708：逐字段拷 m_id/m_itemIdx/m_cnt（0xc 字节），不拷
// m_viewRatio/m_pickRatio（ORIG 反汇编止于 +8）。
CraneItemInfo& CraneItemInfo::operator=(const CraneItemInfo& other)
{
    m_id = other.m_id;
    m_itemIdx = other.m_itemIdx;
    m_cnt = other.m_cnt;
    return *this;
}

CraneMinigameScript::~CraneMinigameScript()
{
    std::vector<CraneItemInfo*>::iterator it = m_items.begin();
    std::vector<CraneItemInfo*>::iterator end = m_items.end();
    for (; it != end; ++it)
    {
        if (*it != 0)
            delete *it;
        *it = 0;
    }
    m_items.clear();
}

CraneMinigameManager::CraneMinigameManager()
    : m_field10(0), m_materials(), m_selectIdx(0)
{
}

CraneMinigameManager::~CraneMinigameManager() {}

// ORIG 0x080ebc6e：reset Action_SendMail 后导入脚本表；失败 trace 并返回 false。
bool CraneMinigameManager::init()
{
    Action_SendMail* mail = &m_sendMail;   // +0x18 内嵌对象直位复位
    mail->m_jobMap.clear();
    mail->m_itemIdx = 0;
    mail->m_addInfo = 0;

    if (!importCraneMinigameScript(
            reinterpret_cast<CraneMinigameScript*>(this),
            "Etc/craneMinigameItem.etc"))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1d, 0);
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

// ORIG 0x080ebf2e（报告 CraneMinigameManager/SendCraneItemNeedMaterial.md）：
// PacketGuard → put_header(0,0x1a7) → put_byte(m_selectIdx +0x68) →
// finalize(true) → CUser::Send。
void CraneMinigameManager::SendCraneItemNeedMaterial(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1a7);
    packet.put_byte(m_selectIdx);
    packet.finalize(true);
    user->Send(packet);
}
