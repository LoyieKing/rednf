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
    g_ServerString_.StrLoading(std::string(file));
    bool result = false;
    strncpy(LETTER_TEXT[0], g_ServerString_.getAuctionString(0, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[1], g_ServerString_.getAuctionString(1, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[2], g_ServerString_.getAuctionString(2, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[3], g_ServerString_.getAuctionString(3, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[4], g_ServerString_.getAuctionString(4, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[5], g_ServerString_.getAuctionString(5, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[6], g_ServerString_.getAuctionString(6, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[7], g_ServerString_.getAuctionString(7, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(LETTER_TEXT[8], g_ServerString_.getAuctionString(8, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(SENDER_NAME, g_ServerString_.getAuctionString(100, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(SENDER_NPC_NAME, g_ServerString_.getAuctionString(101, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    strncpy(SENDER_NAME_GOLD, g_ServerString_.getAuctionString(102, result).c_str(), 0xff);
    if (!result)
    {
        return false;
    }
    return result;
}
