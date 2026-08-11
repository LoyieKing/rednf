// Rebuilt from df_auction_r DWARF (TeaInitialize.cpp CU), 2026-08-08
#include "TeaInitialize.h"
#include "HandlerFor_DB_.h"
#include "TEA.h"

void HandlerFor_DB_::TeaInitialize()
{
    TeaInitialize_taiwan();
}

void HandlerFor_DB_::TeaInitialize_kor()
{
    mTEA.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21,
                    nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_jpn()
{
    mTEA.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21,
                    nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_china()
{
    mTEA.Initialize("Service Management Framework bySCM", 0x21,
                    nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_taiwan()
{
    mTEA.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21,
                    nsl::CTEA::sm_chain0, 0, 0);
}

void HandlerFor_DB_::TeaInitialize_usa()
{
    mTEA.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21,
                    nsl::CTEA::sm_chain0, 0, 0);
}
