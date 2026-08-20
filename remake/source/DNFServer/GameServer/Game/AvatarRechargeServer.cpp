// df_game_r Game/ AvatarRechargeServer 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "AvatarRechargeServer.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "MoreTypes.h"
#include <string.h>

namespace ARAD { namespace DISPATCHER {
    void make_internal_stream_jpn(CStreamGuard& guard, int type, int param)
    {
        guard << (int)0x2dc;
        guard << param;
        guard << type;
    }
}}

int AvatarRechargeServer::GetRateFromEmblemGrade(int grade)
{
    static AvatarRechargeServer s;
    std::map<int, int>::iterator it = s.m_emblemGradeRateMap.find(grade);
    if (it != s.m_emblemGradeRateMap.end())
        return it->second;
    return 0;
}

void AvatarRechargeServer::SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x31, 0xffffffff);
    guard.put_binary((char*)&save, 8);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void AvatarRechargeServer::SaveAvatarEnduranceUnit(int key, int value)
{
    arad::SigAradAvatarEnduranceSave save;
    memset(&save, 0, sizeof(save));
    *(int*)&save = key;
    *((int*)&save + 1) = value;
    SaveAvatarEnduranceUnit(save);
}
