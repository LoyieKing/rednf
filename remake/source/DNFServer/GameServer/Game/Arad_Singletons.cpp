// ============================================================================
// df_game_r 还原 —— ARAD::Singleton<T> 显式实例化（唯一 owner）。
//
// ORIG nm -S --defined-only df_game_r 证据：以下 11 个 ARAD::Singleton<X> 实例
// 存在弱符号 { Singleton() C1/C2, Get(), GetPtr() }（HeroMissionEvent 另有
// GetPtr 0x0811c9c1；CreateDnfEventSciprtServer 另有 Get 0x08164907）。重建侧
// 以模板显式实例化保证符号与 ORIG 一致；本文件为唯一定义点，禁止他处重复。
// ============================================================================

#include "Arad_DataManager.h"        // ARAD::Singleton 模板、ARAD::ScriptManager、
                                     // ARAD::Arad_DataManager

#include "HeroMissionEvent.h"
#include "AvatarConvertServer.h"
#include "GlobalData.h"
#include "RestrictGeolocation.h"     // RestrictGeolocation 权威
#include "ServiceRestrictManager.h"  // ServiceRestrictManager 权威
#include "EmblemCompoundServer.h"
#include "LevelupSupportEventManger.h"
#include "CUser.h"                   // CerashopAddRestrict::Manager

template class ARAD::Singleton<HeroMissionEvent>;
template class ARAD::Singleton<AvatarConvertServer>;
template class ARAD::Singleton<RestrictGeolocation>;
template class ARAD::Singleton<EmblemCompoundServer>;
template class ARAD::Singleton<ServiceRestrictManager>;
template class ARAD::Singleton<LevelupSupportEventManger>;
template class ARAD::Singleton<LevelupSupport2ndEventManger>;
template class ARAD::Singleton<CerashopAddRestrict::Manager>;
template class ARAD::Singleton<ARAD::ScriptManager>;
template class ARAD::Singleton<ARAD::Arad_DataManager>;
