// ============================================================================
// TownAreaScript（G2 Village/区域批次）
// ----------------------------------------------------------------------------
// Village.cpp 以 0x30 布局（int m_areaIndex + char m_pad[0x2c]）承载
// std::map<int, TownAreaScript>（Village::m_areaScripts @ +0x20，0x18 字节 map）
// 并引用构造/拷贝/赋值/析构符号。ORIG 各符号为弱符号：
//   _ZN14TownAreaScriptC1Ev / C2 / D1 / D2 / aSERKS_ / C1ERKS_
// 本 TU 以 Village.cpp 一致的 0x30 POD 语义实现（memset/memcpy），
// 保证 map 节点（0x30 元素）不被越界写入。
// ============================================================================

#include <string.h>

class TownAreaScript
{
public:
    TownAreaScript();
    TownAreaScript(const TownAreaScript& other);
    TownAreaScript& operator=(const TownAreaScript& other);
    ~TownAreaScript();

    int m_areaIndex;   // +0x00
    char m_pad[0x2c];  // +0x04（ORIG string mapName_ 占位；Village 以 0x30 POD 使用）
};

TownAreaScript::TownAreaScript()
{
    memset(this, 0, sizeof(*this));
}

TownAreaScript::TownAreaScript(const TownAreaScript& other)
{
    memcpy(this, &other, sizeof(*this));
}

TownAreaScript& TownAreaScript::operator=(const TownAreaScript& other)
{
    memcpy(this, &other, sizeof(*this));
    return *this;
}

TownAreaScript::~TownAreaScript()
{
}