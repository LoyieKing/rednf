#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "ServerXml.h"
#include "DNFFunctionLib.h"

namespace nsl {
}

char LETTER_TEXT[9][255];
char SENDER_NAME[0xff];
char SENDER_NPC_NAME[0xff];
char SENDER_NAME_GOLD[0xff];

bool initAuctionString(const char* file)
{
    if (file == NULL)
    {
        puts("Invalid file is null");
        return false;
    }
    std::string str(file);
    g_ServerString_.StrLoading(str);
    bool result = false;
    std::string s = g_ServerString_.getAuctionString(0, result);
    strncpy(LETTER_TEXT[0], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(1, result);
    strncpy(LETTER_TEXT[1], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(2, result);
    strncpy(LETTER_TEXT[2], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(3, result);
    strncpy(LETTER_TEXT[3], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(4, result);
    strncpy(LETTER_TEXT[4], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(5, result);
    strncpy(LETTER_TEXT[5], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(6, result);
    strncpy(LETTER_TEXT[6], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(7, result);
    strncpy(LETTER_TEXT[7], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(8, result);
    strncpy(LETTER_TEXT[8], s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(100, result);
    strncpy(SENDER_NAME, s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(101, result);
    strncpy(SENDER_NPC_NAME, s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    s = g_ServerString_.getAuctionString(102, result);
    strncpy(SENDER_NAME_GOLD, s.c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    return result;
}
