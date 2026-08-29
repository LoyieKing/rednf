// PetScript.h —— 宠物脚本（ORIG PetScript::clear 0x08a3dbaa / importPetScript
// 0x08a358d7 逐偏移实证）：
//   +0x00 int m_idx0（importPetScript 尾部 FindIndexByFullScan 写回）
//   +0x04 string m_name（[name]）
//   +0x08/+0x0c int（[width]）
//   +0x10 int（[floating height]）
//   +0x14 int（[layer]：getDrawLayer）
//   +0x18 int（[recovery time]）
//   +0x1c bool（[direction correct]）
//   +0x20..+0x30 5×int（[move speed]）
//   +0x34 int（[gravity]）
//   +0x38 string（[basic motion]）
//   +0x3c vector<string>（[etc motion]）
//   +0x48 vector<string>（[attack info]）
//   +0x54 vector<int>（[int data]）
//   +0x60 vector<string>（[string data]）
//   +0x6c string m_directoryPath（getDirectoryPath "%s%s"）
// clear 硬证据：+0=0；+4=""；+0x8/0xc/0x10=0；+0x14=1；+0x18=0；+0x1c=1；
// 循环清 +0x20..+0x2c；+0x30=0；+0x38=""；+0x3c/0x48/0x54/0x60 clear；+0x6c=""；+0x34=0。
#ifndef DNF_GAME_PETSCRIPT_H_
#define DNF_GAME_PETSCRIPT_H_

#include <string>
#include <vector>

struct PetScript
{
    int m_idx0;                    // +0x00
    std::string m_name;            // +0x04
    int m_width1;                  // +0x08
    int m_width2;                  // +0x0c
    int m_floatingHeight;          // +0x10
    int m_layer;                   // +0x14
    int m_recoveryTime;            // +0x18
    bool m_directionCorrect;       // +0x1c
    int m_moveSpeed[5];            // +0x20..+0x30
    int m_gravity;                 // +0x34
    std::string m_basicMotion;     // +0x38
    std::vector<std::string> m_etcMotion;    // +0x3c
    std::vector<std::string> m_attackInfo;   // +0x48
    std::vector<int> m_intData;              // +0x54
    std::vector<std::string> m_stringData;   // +0x60
    std::string m_directoryPath;   // +0x6c

    void clear();                  // ORIG T 0x08a3dbaa
};

#endif  // DNF_GAME_PETSCRIPT_H_
