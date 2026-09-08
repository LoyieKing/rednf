#include "GlobalScriptTutorial.h"
#include "DNFLexWrapper.h"

#include <string>
#include <vector>

int getCharacterJob(const char* name);

GlobalScriptTutorial::GlobalScriptTutorial()
    : m_data()
{
    clearScript();
}

GlobalScriptTutorial::~GlobalScriptTutorial()
{
    clearScript();
}

// b28d-20: ORIG 0x08ab8e4a。修法：外层/内层扫描循环改 while(true)+`if(!ScanType/ScanStr)break;`
// （P133/P225 负极性短路：ORIG 每条件 xor$1/test + mov$1/$0 汇聚岛 + ebx 循环标志，4.4 对
// `while(ScanType(token,true))` 折成正极性直跳）；token/s 移入循环体（ORIG 每轮构造/析构）；
// 尾部迭代器遍历改索引 size()/operator[] 形（P175）。结果 IDENTICAL_AE（132=133，尾 nop 剥离）。
bool GlobalScriptTutorial::loadScript()
{
    if (!loadRDARScriptFile("Etc/GlobalTutorialInfo/", "CharacterList.lst"))
        return false;

    std::vector<std::string> files;
    while (true)
    {
        std::string token;
        if (!ScanType(token, true))
            break;
        if (token == "[list]")
        {
            while (true)
            {
                std::string s;
                if (!ScanStr(&s))
                    break;
                files.push_back(s);
            }
        }
    }
    unloadRDARScriptFile();

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        if (!loadScript("Etc/GlobalTutorialInfo/", files[i].c_str()))
            return false;
    }
    return true;
}

// b28d-20: ORIG 0x08ab8ffe。修法：token 移入循环体 + while(true)/`if(!ScanType)break;`
// （P0 无槽往返=表达式右值；ORIG 每轮构造/析构 token 并复用 ebx 作循环/返回汇聚标志）。
// 结果 IDENTICAL_AE（155=156，尾 nop 剥离）。
bool GlobalScriptTutorial::loadScript(const char* directory, const char* path)
{
    if (!loadRDARScriptFile(directory, path))
        return false;

    ENUM_CHARACTERJOB job = static_cast<ENUM_CHARACTERJOB>(11);
    while (true)
    {
        std::string token;
        if (!ScanType(token, true))
            break;
        if (token == "[dialog]")
            LoadDialogTag(job);
        else if (token == "[avatar]")
            LoadAvatarTag(job);
        else if (token == "[equipment]")
            LoadEquipmentTag(job);
        else if (token == "[dungeonindex]")
            LoadDungeonIndex(job);
        else if (token == "[skill]")
            LoadSkillTag(job);
        else if (token == "[passive skill]")
            LoadPassiveSkilltag(job);
        else if (token == "[tutorial notice]")
            LoadTutorialNotice(job);
        else if (token == "[item]")
            LoadItem(job);
        else
            job = static_cast<ENUM_CHARACTERJOB>(getCharacterJob(token.c_str()));
    }
    unloadRDARScriptFile();
    return true;
}

// b28d-20: ORIG 0x08ab9226。修法：while(true)+`if(!ScanInt||!ScanInt||!ScanStr)break;`（P133）；
// 槽序 key,face,text —— 4.4 后声明占深槽，ORIG text@-0x14/key@-0xc/face@-0x10（P28 4.4 反向）。
// 结果 IDENTICAL_AE（76=77，尾 nop 剥离）。
void GlobalScriptTutorial::LoadDialogTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, face;
        std::string text;
        if (!ScanInt(&key) || !ScanInt(&face) || !ScanStr(&text))
            break;
        m_data[static_cast<int>(job)].addDialogData(key, face, text.c_str());
    }
}

// b28d-20: ORIG 0x08ab930a。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225，
// ORIG xor$1/test/jne + mov$1/$0 汇聚岛）。结果 IDENTICAL_AE（40=40）。
void GlobalScriptTutorial::LoadAvatarTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int type, index;
    while (true)
    {
        if (!ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addAvatarIndex(type, index);
    }
}

// b28d-20: ORIG 0x08ab937a。修法：①while(true)+`if(!ScanInt×3)break;`（P133）；
// ②实参顺序修正为 addEquipmentIndex(type,index,slot) 且声明序 slot,type,index ——
// ORIG 压栈 arg1=第二扫描值/arg2=第三/arg3=第一（addEquipmentIndex 报告：p1=key,p2=value,
// p3=map 索引且 cmpl $3,p3 越界保护），原实现把第一扫描值当 key 属真语义错位。
// 结果 IDENTICAL_AE（51=51）。
void GlobalScriptTutorial::LoadEquipmentTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, type, index;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addEquipmentIndex(type, index, slot);
    }
}

void GlobalScriptTutorial::LoadDungeonIndex(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    {
    int index;
        for (;;)
{
            if (!ScanInt(&index))
                goto done;
            m_data[static_cast<int>(job)].setDungeonIndex(
                static_cast<unsigned int>(index));
    }
    }
done:
    ;
}

// b28d-20: ORIG 0x08ab9408。修法：while(true)+`if(!ScanInt×5)break;`（P133/P225）。
// 结果 IDENTICAL_AE（71=72，尾 nop 剥离）。
void GlobalScriptTutorial::LoadSkillTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, hotkey, skill, field4, field8;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&hotkey) || !ScanInt(&skill) ||
            !ScanInt(&field4) || !ScanInt(&field8))
            break;
        m_data[static_cast<int>(job)].addSkillIndex(slot, hotkey, skill, field4, field8);
    }
}

// b28d-20: ORIG 0x08ab94d4。修法：while(true)+`if(!ScanInt×3)break;`（P133/P225）。
// 结果 IDENTICAL_AE（51=51）。
void GlobalScriptTutorial::LoadPassiveSkilltag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, skill, field4;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&skill) || !ScanInt(&field4))
            break;
        m_data[static_cast<int>(job)].addPassiveSkillData(slot, skill, field4);
    }
}

// b28d-20: ORIG 0x08ab95ac。修法：while(true)+`if(!ScanInt×3||!ScanStr)break;`（P133）；
// 槽序 key,field8,fieldc,path（4.4 后声明占深槽，ORIG path@-0x18/key@-0xc）。
// 结果 IDENTICAL_AE（84=84）。
void GlobalScriptTutorial::LoadTutorialNotice(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    while (true)
    {
        int key, field8, fieldc;
        std::string path;
        if (!ScanInt(&key) || !ScanInt(&field8) || !ScanInt(&fieldc) || !ScanStr(&path))
            break;
        m_data[static_cast<int>(job)].addTutorialNotice(key, field8, fieldc, path.c_str());
    }
}

// b28d-20: ORIG 0x08ab96a8。修法：while(true)+`if(!ScanInt||!ScanInt)break;`（P133/P225）。
// 结果 IDENTICAL_AE（40=40）。
void GlobalScriptTutorial::LoadItem(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int item, count;
    while (true)
    {
        if (!ScanInt(&item) || !ScanInt(&count))
            break;
        m_data[static_cast<int>(job)].addItem(item, count);
    }
}

void GlobalScriptTutorial::clearScript()
{
}


bool GlobalScriptTutorial::IsGlobalTutorialDungeon(
    register unsigned int dungeon)
{
    if (dungeon == 0xdb2)
        return true;
    return false;
}
