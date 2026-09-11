#ifndef GAME_EXTREAMDUNGEON_H_
#define GAME_EXTREAMDUNGEON_H_

class CEquipItem;
class Inven_Item;

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。

class ExtreamDungeon
{
public:
    // 【推断】类内枚举（ORIG mangled `NS_15eBindSphereType` 实证为 ExtreamDungeon 内 15 字符枚举名）；
    //   值域待从使用点取证后补全。
    enum eBindSphereType
    {
        eBindSphereType_NONE = 0,
    };

    class OUTPUT_ITEM_STR
    {
    public:
        // ORIG 0x832dfbe _ZN14ExtreamDungeon15OUTPUT_ITEM_STRC1Eii (size 0x00000016)  报告: docs/class_func_reports/ExtreamDungeon
        OUTPUT_ITEM_STR(int, int);
        // ORIG 0x8330cec _ZN14ExtreamDungeon15OUTPUT_ITEM_STRD1Ev (size 0x00000005)  报告: docs/class_func_reports/ExtreamDungeon
        ~OUTPUT_ITEM_STR();
    };

    class BindCube_ScriptData
    {
    public:
        // ORIG 0x832e058 _ZN14ExtreamDungeon19BindCube_ScriptDataC1Ev (size 0x000001c8)  报告: docs/class_func_reports/ExtreamDungeon
        BindCube_ScriptData();
        // ORIG 0x832e220 _ZN14ExtreamDungeon19BindCube_ScriptDataD1Ev (size 0x000000f7)  报告: docs/class_func_reports/ExtreamDungeon
        ~BindCube_ScriptData();
    };

    class forceRule_ResultItem
    {
    public:
        // ORIG 0x832e00a _ZN14ExtreamDungeon20forceRule_ResultItemC1Ebi (size 0x00000020)  报告: docs/class_func_reports/ExtreamDungeon
        forceRule_ResultItem(bool, int);
    };

    class CCompound_ExtreamDun_Item
    {
    public:
        // ORIG 0x832c402 _ZN14ExtreamDungeon25CCompound_ExtreamDun_ItemC1Ev (size 0x00000121)  报告: docs/class_func_reports/ExtreamDungeon
        CCompound_ExtreamDun_Item();
        // ORIG 0x832c524 _ZN14ExtreamDungeon25CCompound_ExtreamDun_ItemD1Ev (size 0x00000088)  报告: docs/class_func_reports/ExtreamDungeon
        ~CCompound_ExtreamDun_Item();
        // ORIG 0x832c65e _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item10LoadScriptEPKcS2_ (size 0x00000317)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int LoadScript(char const*, char const*);   // 返回类型【推断】
        // ORIG 0x832de18 _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item13after_addItemEv (size 0x0000014a)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int after_addItem();   // 返回类型【推断】
        // ORIG 0x832db00 _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item15check_N_addItemER10CEquipItem (size 0x0000004d)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int check_N_addItem(CEquipItem&);   // 返回类型【推断】
        // ORIG 0x832ccc0 _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item16makeCompoundItemEPPK10CEquipItemcR10Inven_ItemNS_15eBindSphereTypeE (size 0x0000033e)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int makeCompoundItem(CEquipItem const**, char, Inven_Item&, ExtreamDungeon::eBindSphereType);   // 返回类型【推断】
        // ORIG 0x832d784 _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item27ForceResultItemRule_SettingEibi (size 0x00000052)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int ForceResultItemRule_Setting(int, bool, int);   // 返回类型【推断】
        // ORIG 0x832dd1c _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item8_AddItemER10CEquipItem (size 0x000000fc)  报告: docs/class_func_reports/ExtreamDungeon  【推断:static/成员未定】
        int _AddItem(CEquipItem&);   // 返回类型【推断】
    };

    class BindCube_LvBonusScriptData
    {
    public:
        // ORIG 0x832e318 _ZN14ExtreamDungeon26BindCube_LvBonusScriptDataC1Ev (size 0x00000094)  报告: docs/class_func_reports/ExtreamDungeon
        BindCube_LvBonusScriptData();
        // ORIG 0x832e3ac _ZN14ExtreamDungeon26BindCube_LvBonusScriptDataD1Ev (size 0x0000005f)  报告: docs/class_func_reports/ExtreamDungeon
        ~BindCube_LvBonusScriptData();
    };

    // 实现见 CItemList.h/.cpp（namespace ExtreamDungeon 级自由函数，mangled 与
    // ORIG 一致；CItemList.h:336 的 OUTPUT_ITEM_STR 即 ExtreamDungeon::OUTPUT_ITEM_STR）。
    // bool BindTable_Predicate(OUTPUT_ITEM_STR const& a, OUTPUT_ITEM_STR const& b);
};

#endif  // GAME_EXTREAMDUNGEON_H_
