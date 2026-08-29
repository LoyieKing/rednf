// AvatarVariation 域自由函数声明（ORIG 符号均为 namespace 自由函数，非类成员）。
// 类型权威定义见 CDataManager.h：namespace AvatarVariation { struct colorRGB; class AvatarColorInfo; }
// 实现见 AvatarVariation.cpp（ORIG 0x088d43f0..0x088d495b）。
#ifndef DNF_GAME_AVATARVARIATION_H_
#define DNF_GAME_AVATARVARIATION_H_

namespace AvatarVariation
{

struct colorRGB;          // 完整定义见 CDataManager.h（含 ctor/operator==）
class AvatarColorInfo;    // 完整定义见 CDataManager.h

// ORIG T 0x088d46d3：函数局部 static AvatarColorInfo（P20/P21 guard 语义），
// 每次调用经 importAvatarColorVariation 装载；失败返回 0。
AvatarColorInfo* getAvatarColorInfoInst();

// ORIG T 0x088d4782：key∈[0,0x1ff] 查 m_colorMap；缺省返回 colorRGB(0xff,0xff,0xff)。
// 按值返回（i386 POD 3 字节走 sret；ORIG 调用侧实证）。
colorRGB getAvatarColorValue(int key);

// ORIG T 0x088d4856：与 getAvatarColorValue(0) 相等则不可用（返回 !(color==default)）。
bool isEnAbleColorKey(colorRGB& color);

// ORIG T 0x088d4896：key∈[0,0x1ff] 查 m_nameMap 返回 c_str；失败返回 0。
const char* getAvatarColorName(int key);

// ORIG T 0x088d444f（定义见 InitScriptProducers_8.cpp）：装载 "Etc/AvatarColorVariation.etc"。
bool importAvatarColorVariation(AvatarColorInfo* out);

}  // namespace AvatarVariation

#endif // DNF_GAME_AVATARVARIATION_H_
