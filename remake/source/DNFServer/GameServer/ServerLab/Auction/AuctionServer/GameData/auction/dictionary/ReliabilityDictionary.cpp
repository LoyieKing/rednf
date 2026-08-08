// Rebuilt from df_auction_r DWARF (ReliabilityDictionary.cpp CU), 2026-08-08
#include <iostream>

#include "ReliabilityDictionary.h"
#include "AuctionItem.h"

struct STGuildCargoLog
{
    int m_iTime;               // @0
    char m_cBehavior;          // @4
    char m_szCharacName[33];   // @5
    int m_iItemID;             // @0x26
    int m_iEtc;                // @0x30
    RandomOption m_randomOption;  // @0x34
};

static bool comp_by_time(const STGuildCargoLog& left, const STGuildCargoLog& right)
{
    return right.m_iTime < left.m_iTime;
}

ReliabilityDictionary::ReliabilityDictionary()
{
}

ReliabilityDictionary::~ReliabilityDictionary()
{
}
