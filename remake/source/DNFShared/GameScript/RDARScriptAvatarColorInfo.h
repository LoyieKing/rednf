#ifndef RDARSCRIPTAVATARCOLORINFO_H_
#define RDARSCRIPTAVATARCOLORINFO_H_

#include <map>
#include <string>

#include "stdafx.h"

namespace AvatarVariation
{

typedef unsigned char byte;

struct colorRGB
{
    colorRGB();
    bool operator==(const colorRGB& rhs);

    byte red;
    byte green;
    byte blue;
};

typedef std::map<int, colorRGB> AvatarColorMap;
typedef std::map<int, std::string> ColorNameMap;

class AvatarColorInfo
{
public:
    AvatarColorInfo();
    ~AvatarColorInfo();

    AvatarColorMap avatarColorMap;
    ColorNameMap avatarColorNameMap;
};

void bracketTrim(char* data, char type, char changeType);
bool Parse_Table(char* in_pcReadData, int iLineCount, AvatarColorInfo* avatarColorInfo);
bool importAvatarColorVariation(AvatarColorInfo* avatarColorInfo);
AvatarColorInfo* getAvatarColorInfoInst();
colorRGB getAvatarColorValue(int index);
bool isEnAbleColorKey(colorRGB& checkColor);
char* getAvatarColorName(int index);

} // namespace AvatarVariation

#endif // RDARSCRIPTAVATARCOLORINFO_H_
