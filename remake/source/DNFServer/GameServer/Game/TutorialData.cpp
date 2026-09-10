// TutorialData — df_game_r ORIG 转写（docs/class_func_reports/TutorialData/*）
// 新 TU：需在 source/cmake/game/CMakeLists.txt 登记（宿主收口）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
#include <iostream>

#include "TutorialData.h"

#include "GlobalScriptTutorial.h"   // GlobalScriptTutorial::getData（ORIG 同 TU 域 0x08ab5xxx-0x08ab9xxx；被调前置定义使 4.4 帧收窄为 sub $0x4，与 ORIG 0x08ab971e 一致）

#include <cstddef>

static_assert(sizeof(TutorialData) == 0x148,
              "TutorialData layout mismatch vs ORIG (0x148)");
static_assert(sizeof(TutorialData::DialogData) == 8,
              "DialogData layout mismatch vs ORIG (8)");
static_assert(sizeof(TutorialData::SkillData) == 12,
              "SkillData layout mismatch vs ORIG (0xc)");
static_assert(sizeof(TutorialData::TutorialNotice) == 0x10,
              "TutorialNotice layout mismatch vs ORIG (0x10)");
static_assert(sizeof(TutorialData::ItemData) == 8,
              "ItemData layout mismatch vs ORIG (8)");

// ---- 子对象生命周期（ORIG W） ----------------------------------------

// ORIG _ZN12TutorialDataC1Ev @0x08ab7e4c：
// 容器按成员顺序构造（各数组循环展开），随后 *this=0，
// 最后对 avatarIndexMap 额外执行一次 clear()。
// [A31-GI · P327] 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
TutorialData::TutorialData()
{
    // 成员由编译器按声明顺序构造
    m_dungeonIndex = 0;
    m_avatarIndexMap.clear();
}

// ORIG _ZN12TutorialData10DialogDataC1Ev @0x08ab981e：
// 仅默认构造 this+0 的 std::string；m_faceIndex 不初始化。
TutorialData::DialogData::DialogData()
    : m_str()
{
}

// ORIG _ZN12TutorialData14TutorialNoticeC1Ev @0x08ab985e：
// 仅默认构造 this+4 的 std::string；其余 int 成员不初始化。
TutorialData::TutorialNotice::TutorialNotice()
    : m_aniPath()
{
}

// ---- 主类生命周期 -----------------------------------------------------

// ORIG _ZN12TutorialDataD1Ev @0x08ab80ee：成员逆序析构，无额外逻辑。
TutorialData::~TutorialData()
{
}

// ---- dungeonIndex ------------------------------------------------------

// ORIG _ZN12TutorialData15setDungeonIndexEj @0x08ab82c4
void TutorialData::setDungeonIndex(unsigned int index)
{
    m_dungeonIndex = index;
}

// ORIG _ZN12TutorialData15getDungeonIndexEv @0x08ab82d2
int TutorialData::getDungeonIndex()
{
    return static_cast<int>(m_dungeonIndex);
}

// ORIG _ZN20GlobalScriptTutorial7getDataEi @0x08ab971e（报告 getData.md 已读）
// 越界返回 0；m_data[index].getDungeonIndex()==0 返回 0，否则返回该元素地址。
// N96 清绑：原 `register bool isZero asm("al")` 已删。ORIG isZero 无槽
// （test %eax;sete %al;test %al,%al;je）⇒ 按 P226扩展/P316① 用语句表达式包住条件。
// 清绑前后 25=25、与 ORIG 逐条全等（仅 .o 重定位地址差）⇒ 残差 0；
// 具名 bool 局部形为 26（+1 槽物化）。
TutorialData* GlobalScriptTutorial::getData(int index)
{
    if (!(index < 11 && index >= 0))
        return 0;
    {
        if (({ m_data[index].getDungeonIndex() == 0; }))
            return 0;
        return &m_data[index];
    }
}

// ---- 对话数据 ----------------------------------------------------------

// ORIG _ZN12TutorialData13addDialogDataEiiPKc @0x08ab82dc
void TutorialData::addDialogData(int key, int faceIndex, char const* text)
{
    DialogMap::iterator it = m_dialogs.find(key);

    DialogData* data = new DialogData;
    data->m_faceIndex = faceIndex;
    data->m_str = text;

    if (it != m_dialogs.end()) {
        it->second.push_back(data);
        return;
    }

    std::vector<DialogData*> entries;
    entries.push_back(data);
    m_dialogs.insert(std::make_pair(key, entries));
}

// ORIG _ZN12TutorialData13getDialogSizeEj @0x08ab8486
unsigned int TutorialData::getDialogSize(unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    return static_cast<unsigned int>(it->second.size());
}

// ORIG _ZN12TutorialData12getDialogStrEjj @0x08ab852e
// ORIG 反汇编：map.find(0x10=param2)、vector.at(0xc=param1)、
// 越界比较 param1 < size —— 即 param1 为 vector 索引、param2 为 map key。
char const* TutorialData::getDialogStr(unsigned int idx, unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    // ORIG 0x08ab85a4：`cmp 0xc(%ebp),%eax; seta %al; test %al,%al; je 0-岛`
    // —— 正极性 `if (idx < size)` 才让 4.4 出 seta + 成功路径 fall-through
    // （写 !(idx<size) 早返出 setbe 且块序相反，P220）。
    if (idx < it->second.size()) {
        return it->second.at(idx)->m_str.c_str();
    }
    return 0;
}

// ORIG _ZN12TutorialData12getFaceIndexEjj @0x08ab85de（参数语义同 getDialogStr）。
int TutorialData::getFaceIndex(unsigned int idx, unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    // 同 getDialogStr：ORIG 0x08ab8654 seta 物化 + 正极性布局。
    if (idx < it->second.size()) {
        return it->second.at(idx)->m_faceIndex;
    }
    return 0;
}

// ORIG _ZN12TutorialData16isCurrentEpisodeEjj @0x08ab84fe
bool TutorialData::isCurrentEpisode(unsigned int episodeNo, unsigned int count)
{
    if (episodeNo < getDialogSize(count)) {
        return true;
    }
    return false;
}

// ---- 头像索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData14addAvatarIndexEii @0x08ab868a：
// make_pair(p1,p2) 插入 this+0x1c。
void TutorialData::addAvatarIndex(int type, int index)
{
    m_avatarIndexMap.insert(std::make_pair(type, index));
}

// ORIG _ZN12TutorialData14getAvatarIndexE18ENUM_EQUIPMENTTYPE @0x08ab874e
int TutorialData::getAvatarIndex(ENUM_EQUIPMENTTYPE type)
{
    std::map<int, int>::iterator it =
        m_avatarIndexMap.find(static_cast<int>(type));
    if (it != m_avatarIndexMap.end()) {
        return it->second;
    }
    return -1;
}

// ---- 装备索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData17addEquipmentIndexEiii @0x08ab86e0：
// p3<4 门控；make_pair(p1,p2) 插入 m_equipmentMaps[p3]。
void TutorialData::addEquipmentIndex(int type, int index, int slot)
{
    if (slot >= 4) {
        return;
    }
    m_equipmentMaps[slot].insert(std::make_pair(type, index));
}

// ORIG _ZN12TutorialData17getEquipmentIndexE18ENUM_EQUIPMENTTYPEi @0x08ab87be
int TutorialData::getEquipmentIndex(ENUM_EQUIPMENTTYPE type, int slot)
{
    if (slot >= 4) {
        return -1;
    }
    std::map<int, int>::iterator it =
        m_equipmentMaps[slot].find(static_cast<int>(type));
    if (it != m_equipmentMaps[slot].end()) {
        return it->second;
    }
    return -1;
}

// ---- 技能索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData16getConvertHotKeyEi @0x08ab885c
int TutorialData::getConvertHotKey(int hotKey)
{
    if (hotKey < 0 || hotKey > 12) {
        return -1;
    }
    if (hotKey <= 5) {
        return hotKey;
    }
    return hotKey + 0xc0;
}

// ORIG _ZN12TutorialData13addSkillIndexEiiiii @0x08ab8888：
// p1<4 门控；SkillData{+4:p4,+0:p3,+8:p5}（ORIG 写序）；键=getConvertHotKey(p2)；
// 键为 -1 时丢弃（按 ORIG 不释放）；插入 m_skillMaps[p1]。
void TutorialData::addSkillIndex(int slot, int hotKey,
                                 int skillIndex, int field4, int field8)
{
    if (slot > 3) {
        return;
    }

    SkillData* skill = new SkillData;
    skill->m_field4 = field4;
    skill->m_field0 = skillIndex;
    skill->m_field8 = field8;

    int convertedKey = getConvertHotKey(hotKey);
    if (convertedKey == -1) {
        return;
    }

    m_skillMaps[slot].insert(
        std::make_pair(convertedKey, skill));
}

// ORIG _ZN12TutorialData12getSkillDataEii @0x08ab89fa：
// 用 p2 原值查 m_skillMaps[p1]（与插入的转换键不同口径，忠实转写）。
TutorialData::SkillData* TutorialData::getSkillData(int slot, int hotKey)
{
    std::map<int, SkillData*>::iterator it = m_skillMaps[slot].find(hotKey);
    if (it != m_skillMaps[slot].end()) {
        return it->second;
    }
    return 0;
}

// ORIG _ZN12TutorialData19addPassiveSkillDataEiii @0x08ab8944：
// p1<4 门控；SkillData{+4:p3,+0:p2,+8:-1}（ORIG 写序）push 入 m_passiveSkills[p1]。
void TutorialData::addPassiveSkillData(int slot, int skillIndex, int field4)
{
    if (slot >= 4) {
        return;
    }
    SkillData* skill = new SkillData;
    skill->m_field4 = field4;
    skill->m_field0 = skillIndex;
    skill->m_field8 = -1;
    m_passiveSkills[slot].push_back(skill);
}

// ORIG _ZN12TutorialData19getPassiveSkillDataEi @0x08ab89a6（sret）：
// p1 越界返回空副本；命中返回成员向量副本。
std::vector<TutorialData::SkillData*> TutorialData::getPassiveSkillData(int slot)
{
    if (slot < 0 || slot > 3) {
        return std::vector<SkillData*>();
    }
    return m_passiveSkills[slot];
}

// ---- 物品 -------------------------------------------------------------

// ORIG _ZN12TutorialData7addItemEii @0x08ab8d1c
void TutorialData::addItem(int itemId, int count)
{
    ItemData* item = new ItemData;
    item->m_itemId = itemId;
    item->m_count = count;
    m_items.push_back(item);
}

// ORIG _ZN12TutorialData7getItemEv @0x08ab8d5c（sret）
std::vector<TutorialData::ItemData*> TutorialData::getItem()
{
    return m_items;
}

// ---- 教程公告 ---------------------------------------------------------

// ORIG _ZN12TutorialData17addTutorialNoticeEiiiPKc @0x08ab8a86：
// TutorialNotice{+0:p1,+4:p4 文本,+8:p2,+c:p3}；make_pair(p1, vec) 插入 m_notices。
void TutorialData::addTutorialNotice(int key, int field8, int fieldc,
                                     char const* path)
{
    NoticeMap::iterator it = m_notices.find(key);

    TutorialNotice* notice = new TutorialNotice;
    notice->m_field0 = key;
    notice->m_aniPath = path;
    notice->m_field8 = field8;
    notice->m_fieldc = fieldc;

    if (it != m_notices.end()) {
        it->second.push_back(notice);
        return;
    }

    std::vector<TutorialNotice*> entries;
    entries.push_back(notice);
    m_notices.insert(std::make_pair(key, entries));
}

// ORIG _ZN12TutorialData16getNoticeAniPathEi @0x08ab8c4c（sret）：
// 未命中复制空临时向量。
std::vector<TutorialData::TutorialNotice*> TutorialData::getNoticeAniPath(int key)
{
    std::vector<TutorialNotice*> empty;
    NoticeMap::iterator it = m_notices.find(key);
    // ORIG 0x08ab8cb3：`call ne; test %al,%al; je 0x87` ⇒ 源码用 != 正极性
    // （it!=end 走 fall 返回 it->second）；empty 分支 ORIG 0x08ab8cdf 调
    // vector::C1EOS4_（move 构造，GCC4.4.7 不隐式 move 局部返回值 ⇒ 显式
    // std::move，否则生成 C1ERKS4_ 拷贝 +5 条）。
    if (it != m_notices.end()) {
        return it->second;
    }
    // ORIG 0x08ab8cdf 调 vector::C1EOS4_（move 构造）。用 static_cast 到右值
    // 引用而非 std::move()——4.4.7 -O0 把 std::move 模板实例化为 out-of-line
    // call（多 1 条 call + 传参），ORIG 无该 call（4.1 内联该转换）。
    return static_cast<std::vector<TutorialNotice*>&&>(empty);
}
