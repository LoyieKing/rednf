// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "RDARScriptAvatarColorInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "DNFFunctionLib.h"

namespace AvatarVariation
{

colorRGB::colorRGB()
{
    red = 0xff;
    green = 0xff;
    blue = 0xff;
}

bool colorRGB::operator==(const colorRGB& rhs)
{
    if ((red == rhs.red) && (green == rhs.green) && (blue == rhs.blue))
    {
        return true;
    }
    return false;
}

AvatarColorInfo::AvatarColorInfo()
{
}

AvatarColorInfo::~AvatarColorInfo()
{
}

void bracketTrim(char* data, char type, char changeType)
{
    int i = 0;
    while (data[i] != '\0')
    {
        if (data[i] == type)
        {
            data[i] = changeType;
            return;
        }
        i = i + 1;
    }
}

bool Parse_Table(char* in_pcReadData, int iLineCount, AvatarColorInfo* avatarColorInfo)
{
    int AVATAR_COLOR_SCRIPT = 5;
    char* pcColumn_Arg[5];

    if ((*in_pcReadData == '#') || (*in_pcReadData == '/') || (*in_pcReadData == '['))
    {
        return false;
    }
    if (DNFFLib::ExplodeString(in_pcReadData, " \t\r\n\"", pcColumn_Arg, 5) == 5 &&
        iLineCount <= 0xfe)
    {
        colorRGB temColor;
        std::string name;
        int intData;
        intData = 0;
        intData = atoi(pcColumn_Arg[0]);
        temColor.red = (byte)atoi(pcColumn_Arg[1]);
        temColor.green = (byte)atoi(pcColumn_Arg[2]);
        temColor.blue = (byte)atoi(pcColumn_Arg[3]);
        bracketTrim(pcColumn_Arg[4], '`', '(');
        bracketTrim(pcColumn_Arg[4], '`', ')');
        name = pcColumn_Arg[4];
        avatarColorInfo->avatarColorMap.insert(std::make_pair(intData, temColor));
        avatarColorInfo->avatarColorNameMap.insert(std::make_pair(intData, name));
        return true;
    }
    return false;
}

bool importAvatarColorVariation(AvatarColorInfo* avatarColorInfo)
{
    int iParseCount = 0;
    char cReadData[1024];
    FILE* pFile = fopen("AvatarColorVariation.etc", "rb");
    if (pFile == 0)
    {
        return false;
    }
    avatarColorInfo->avatarColorMap.clear();
    while (true)
    {
        if (feof(pFile) == 0 && fgets(cReadData, 0x400, pFile) != (char*)0)
        {
        }
        else
        {
            break;
        }
        if (cReadData[0] != '#' && cReadData[0] != '/')
        {
            if (0xfe < iParseCount)
            {
                return false;
            }
            if (Parse_Table(cReadData, iParseCount, avatarColorInfo))
            {
                iParseCount = iParseCount + 1;
            }
        }
    }
    fclose(pFile);
    return true;
}

AvatarColorInfo* getAvatarColorInfoInst()
{
    static AvatarColorInfo s_avatarColorInfo;
    bool bResult = importAvatarColorVariation(&s_avatarColorInfo);
    if (bResult)
    {
        return &s_avatarColorInfo;
    }
    return (AvatarColorInfo*)0;
}

colorRGB getAvatarColorValue(int index)
{
    if (index < 0 || 0x1ff < index)
    {
        index = 0;
    }
    AvatarColorInfo* tempMap = getAvatarColorInfoInst();
    if (tempMap == (AvatarColorInfo*)0)
    {
        return colorRGB();
    }
    AvatarColorMap::const_iterator itr = tempMap->avatarColorMap.find(index);
    if (itr != tempMap->avatarColorMap.end())
    {
        return itr->second;
    }
    return colorRGB();
}

bool isEnAbleColorKey(colorRGB& checkColor)
{
    colorRGB local_f = getAvatarColorValue(0);
    return (checkColor == local_f) ? false : true;
}

char* getAvatarColorName(int index)
{
    if ((index < 0) || (0x1ff < index))
    {
        return (char*)0;
    }
    ColorNameMap* tempMap = &getAvatarColorInfoInst()->avatarColorNameMap;
    if (tempMap == (ColorNameMap*)0)
    {
        return (char*)0;
    }
    ColorNameMap::const_iterator itr = tempMap->find(index);
    if (itr != tempMap->end())
    {
        return (char*)itr->second.c_str();
    }
    return (char*)0;
}

} // namespace AvatarVariation
