// ORIG T 0x0853855c（报告转写）：ImportWorldDrop(this+0x18, path) 失败返回
// 0x7b9；ImportWorldDrop(this+0xfcc, warArea) 失败返回 0x7bb；成功返回 0。
// 注：DataManagerInitDepends.h 既存声明第二参为 char*（ORIG mangled 实为
// PKc），属 InitScriptProducers_1 域既存 ABI 偏差，此处 const_cast 桥接。
int WongWork::CWorldDrop::loadScript(char const* path, char const* warArea)
{
    if (!ImportWorldDrop(m_worldDrop, const_cast<char*>(path)))
        return 0x7b9;
    if (!ImportWorldDrop(m_warAreaDrop, const_cast<char*>(warArea)))
        return 0x7bb;
    return 0;
}