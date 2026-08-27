#ifndef DNF_TUTORIAL_DATA_H
#define DNF_TUTORIAL_DATA_H

// ORIG: TutorialData @ df_game_r 0x08ab7e4c..0x08ab80ed (ctor), ~ 0x08ab80ee
// sizeof(TutorialData) == 0x148（GlobalScriptTutorial 以 0x148 步长持有数组，
// 见 docs/class_func_reports/GlobalScriptTutorial/LoadDialogTag.md）
// 布局（完整读 docs/class_func_reports/TutorialData/* 恢复；工具链探测：
// std::string=4 / vector=12 / map=24）：
//   +0x000 unsigned int      m_dungeonIndex          (setDungeonIndex/getDungeonIndex)
//   +0x004 DialogMap         m_dialogs               map<int, vector<DialogData*>>   (0x18)
//   +0x01c IntMap            m_avatarIndexMap        map<int,int>
//   +0x034 IntMap            m_equipmentMaps[4]      (stride 0x18 → +0x94)
//   +0x094 SkillMap          m_skillMaps[4]          map<int, SkillData*> (→ +0xf4)
//   +0x0f4 PassiveVec        m_passiveSkills[4]      vector<SkillData*> (stride 0xc → +0x124)
//   +0x124 NoticeMap         m_notices               map<int, vector<TutorialNotice*>> (0x18)
//   +0x13c ItemVec           m_items                 vector<ItemData*>

#include <map>
#include <string>
#include <vector>

#ifndef DNF_ENUM_EQUIPMENTTYPE_DEFINED
#define DNF_ENUM_EQUIPMENTTYPE_DEFINED
enum ENUM_EQUIPMENTTYPE
{
    ENUM_EQUIPMENTTYPE_0 = 0
};
#endif

class TutorialData
{
public:
    // ORIG _ZN12TutorialData10DialogDataC1Ev @0x08ab981e（仅默认构造 string@+0；
    // new 大小 8）
    struct DialogData
    {
        std::string m_str;       // +0x00（addDialogData 的 dialog 文本）
        int m_faceIndex;         // +0x04（addDialogData: p2）

        DialogData();            // ORIG W 0x08ab981e
    };

    struct SkillData
    {
        int m_field0;            // +0x00（addSkillIndex: p3；addPassiveSkillData: p2）
        int m_field4;            // +0x04（addSkillIndex: p4；addPassiveSkillData: p3）
        int m_field8;            // +0x08（addSkillIndex: p5；addPassiveSkillData 固定 -1）
    };

    struct ItemData
    {
        int m_itemId;            // +0x00（addItem: p1）
        int m_count;             // +0x04（addItem: p2）
    };

    // ORIG _ZN12TutorialData14TutorialNoticeC1Ev @0x08ab985e（仅默认构造 string@+4；
    // new 大小 0x10）
    struct TutorialNotice
    {
        int m_field0;            // +0x00（addTutorialNotice: p1）
        std::string m_aniPath;   // +0x04（addTutorialNotice: p4 文本）
        int m_field8;            // +0x08（addTutorialNotice: p2）
        int m_fieldc;            // +0x0c（addTutorialNotice: p3）

        TutorialNotice();        // ORIG W 0x08ab985e
    };

    typedef std::map<int, std::vector<DialogData*> > DialogMap;
    typedef std::map<int, int> IntMap;
    typedef std::map<int, SkillData*> SkillMap;
    typedef std::map<int, std::vector<TutorialNotice*> > NoticeMap;

    TutorialData();              // ORIG T 0x08ab7e4c
    ~TutorialData();             // ORIG T 0x08ab80ee

    void setDungeonIndex(unsigned int);                      // ORIG T 0x08ab82c4
    int getDungeonIndex();                                   // ORIG T 0x08ab82d2

    void addDialogData(int, int, char const*);               // ORIG T 0x08ab82dc
    unsigned int getDialogSize(unsigned int);                // ORIG T 0x08ab8486
    char const* getDialogStr(unsigned int, unsigned int);    // ORIG T 0x08ab852e
    int getFaceIndex(unsigned int, unsigned int);            // ORIG T 0x08ab85de
    bool isCurrentEpisode(unsigned int, unsigned int);       // ORIG T 0x08ab84fe

    void addAvatarIndex(int, int);                           // ORIG T 0x08ab868a
    int getAvatarIndex(ENUM_EQUIPMENTTYPE);                  // ORIG T 0x08ab874e

    void addEquipmentIndex(int, int, int);                   // ORIG T 0x08ab86e0
    int getEquipmentIndex(ENUM_EQUIPMENTTYPE, int);          // ORIG T 0x08ab87be

    void addSkillIndex(int, int, int, int, int);             // ORIG T 0x08ab8888
    SkillData* getSkillData(int, int);                       // ORIG T 0x08ab89fa
    void addPassiveSkillData(int, int, int);                 // ORIG T 0x08ab8944
    std::vector<SkillData*> getPassiveSkillData(int);        // ORIG T 0x08ab89a6

    void addItem(int, int);                                  // ORIG T 0x08ab8d1c
    std::vector<ItemData*> getItem();                        // ORIG T 0x08ab8d5c

    void addTutorialNotice(int, int, int, char const*);      // ORIG T 0x08ab8a86
    std::vector<TutorialNotice*> getNoticeAniPath(int);      // ORIG T 0x08ab8c4c

    int getConvertHotKey(int);                               // ORIG T 0x08ab885c

private:
    unsigned int m_dungeonIndex;
    DialogMap m_dialogs;
    IntMap m_avatarIndexMap;
    IntMap m_equipmentMaps[4];
    SkillMap m_skillMaps[4];
    std::vector<SkillData*> m_passiveSkills[4];
    NoticeMap m_notices;
    std::vector<ItemData*> m_items;

    // ORIG: 这四组指针容器仅析构容器本身，元素按 ORIG 行为泄漏，不补 delete。
};

#endif // DNF_TUTORIAL_DATA_H
