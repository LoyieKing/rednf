// Rebuilt from df_auction_r DWARF (TeaInitialize.cpp CU), 2026-08-08
#include <string.h>

#include "TeaInitialize.h"
#include "HandlerFor_DB_.h"
#include "TEA.h"

namespace {

const char g_teaKeyKor[] = "qortmddkqortmdcksqordudwlswjdguswn";
const char g_teaKeyChina[] = "Service Management Framework bySCM";

} // namespace

void HandlerFor_DB_::TeaInitialize()
{
    TeaInitialize_taiwan();
}

void HandlerFor_DB_::TeaInitialize_kor()
{
    mTEA.Initialize(g_teaKeyKor, 0x21, nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_jpn()
{
    mTEA.Initialize(g_teaKeyKor, 0x21, nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_china()
{
    mTEA.Initialize(g_teaKeyChina, 0x21, nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_taiwan()
{
    mTEA.Initialize(g_teaKeyKor, 0x21, nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_usa()
{
    mTEA.Initialize(g_teaKeyKor, 0x21, nsl::CTEA::sm_chain0, 0, 0);
}
