#ifndef GAME_SCRIPT_ENUM_HELPERS_H
#define GAME_SCRIPT_ENUM_HELPERS_H

// GameScriptEnumHelpers.h — 字符串→枚举映射自由函数（脚本导入链使用）。
// 全部签名与 ORIG df_game_r mangled 符号一致；实现见 GameScriptEnumHelpers.cpp。

#include <string>

int getStateToEnumValue(std::string& s);              // 0x8892b0d 族外：0x89bbc05，恒 -1
int getActiveStatusToEnumValue(std::string& s);       // 0x88c29a2
int getTargetToEnumValue(std::string& s);             // 0x88c2c34
int getAttackConditionToEnumValue(std::string& s);    // 0x89bcb6d
int getCategoryToEnumValue(std::string& s);           // 0x89bcbf2
int getElementToEnumValue(std::string& s);            // 0x89bca42
int getGradeToEnumValue(std::string& s);              // 0x89bcd5b
int getObjectTypeToEnumValue(std::string& s);         // 0x89bbd99
int getChangeStatusType(std::string& s);              // 0x8892b0d
int getActivePropertyTypeToEnumValue(std::string& s); // ORIG 0x089bbe1e.
int getNpcRoleByString(std::string name);             // 0x89fecee（按值传参，与 NpcScript.h 一致）
int getCharacterJobToEnumValue(std::string& s);      // Existing ORIG symbol.
int getAvatarEmblemType(const char* name);           // ORIG 0x08990f66.
int GetEquipmentType(const char* name);              // ORIG 0x08990f1f.
int getBuffToEnumValue(std::string& s);              // Existing ORIG symbol.
int getEquipmentTypeToEnumValue(std::string& s);     // Existing ORIG symbol.
int getStackableIncreaseStatusType(std::string& s);  // ORIG 0x089aee71.
int getExcepttionStatusToEnumValue(std::string& s);  // ORIG 0x089bcde0.
int scanWorldType();                                 // ORIG 0x088c1430.
int getNpcMoodFromString(const char* name);           // 0x89faf46

int scanModuleType();                                 // 0x88c0b9a（ORIG 返回 int）
void setCodePageByLocal();                            // 0x808ec64
int setCodePage(int nNewCodePage);                    // 0x8adea1a（ORIG 同体提供）

#endif // GAME_SCRIPT_ENUM_HELPERS_H
