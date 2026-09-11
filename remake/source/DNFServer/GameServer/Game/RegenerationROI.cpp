// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 OnlyOrigBatch11.cpp 拆出。
//   ORIG TU     ：RegenerationROI.cpp
//   ORIG 期望键：_GLOBAL__I__Z16_GetRandomOptionv
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/stat.h>   // umask
#include <unistd.h>     // fork/setsid/chdir
#include <map>
#include <string>
#include <vector>
#include <strings.h>      // strcasecmp
#include "ThreadInterface.h"
#include "StrikerScript.h"   // TagSkillPenalty / StrikerDirectionType
#include "UPGRADEJAR.h"
#include "GlobalData.h"      // s_systemTime_（get_str_time）
#include "GameRand.h"        // get_rand_int
#include "ScriptValue.h"     // ScriptValue
#include "StringResource.h"  // CStringManager::getStr（getAvatarRentDayString）
#include "Quest.h"           // ENUM_CHARACTERJOB
#include "LogManager.h"     // convert_string_to_time catch 日志
#include "STStackableScript.h"   // ENUM_MODULE_TYPE（DataManagerInitDepends.h 依赖）
#include "DataManagerInitDepends.h"  // ENUM_ACTIVESTATUS / RandomOptionScript 视图
#include "CDataManager.h"    // G_CDataManager + m_randomOption（_GetRandomOption）
#include "StringTypes.h"   // CharString / CharStringData（convertToUtf8）
#include <boost/date_time/posix_time/posix_time.hpp>  // convert_string_to_time

RandomOptionScript& _GetRandomOption()
{
    return G_CDataManager()->m_randomOption;
}
