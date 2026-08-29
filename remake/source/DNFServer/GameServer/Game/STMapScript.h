#ifndef DNF_GAME_STMAPSCRIPT_H_
#define DNF_GAME_STMAPSCRIPT_H_

#include "CMap.h"   // ENUM_DRAWLAYER

// STMapScript 域权威枚举（原 STMapScript.cpp TU 局部；ORIG 为全局类型——
// mangled ENUM_MAP_FOLLOW_TYPE/ENUM_PASSAGETYPE 实证，跨 TU 消费需可见）。

enum ENUM_MAP_FOLLOW_TYPE
{
    MAP_FOLLOW_TYPE_PATH_GATE = 1,     // "[path gate]"
    MAP_FOLLOW_TYPE_PARENT = 2,        // "[parent]"
    MAP_FOLLOW_TYPE_FROM_PARENT = 3,   // "[from parent]"
    MAP_FOLLOW_TYPE_KEEP_POSITION = 4, // "[keep position]"
};

enum ENUM_PASSAGETYPE
{
    PASSAGETYPE_PASS_ALL = 0,               // "[pass all]"
    PASSAGETYPE_PASS_ONLY_FLOAT_TYPE = 1,   // "[pass only float type]"
    PASSAGETYPE_DO_NOT_PASS = 2,            // "[do not pass]"
    PASSAGETYPE_PASS_ONLY_RED_TEAM = 3,     // "[pass only red team]"
    PASSAGETYPE_PASS_ONLY_BLUE_TEAM = 4,    // "[pass only blue team]"
    PASSAGETYPE_PASS_ONLY_CHARACTER = 7,    // "[pass only character]"
};

// 名字查询（ORIG T 0x088be212/0x088be343/0x088be3e7；定义 STMapScript.cpp）
const char* getDrawLayerName(ENUM_DRAWLAYER drawLayer);
const char* getFollowTypeName(ENUM_MAP_FOLLOW_TYPE followType);
const char* getPassageTypeName(ENUM_PASSAGETYPE passageType);

#endif  // DNF_GAME_STMAPSCRIPT_H_
