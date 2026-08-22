// ============================================================================
// CItemGeneratorMgr.h — WongWork::CItemGeneratorMgr 权威声明
//
// ORIG 反汇编证据（df_game_r, elf32-i386）：
//   WongWork::CItemGeneratorMgr::getGenerator(eGenerateType_t) @ 0x0814a7d0
//       symbol: _ZN8WongWork17CItemGeneratorMgr12getGeneratorENS0_15eGenerateType_tE
//       mov 0xc(%ebp),%edx        ; edx = type
//       mov 0x8(%ebp),%eax        ; eax = this
//       add $0xf0,%edx            ; edx = type + 0xf0
//       mov (%eax,%edx,4),%eax    ; eax = *(this + (type+0xf0)*4)
//       => 数组基址字节偏移 0x3c0 = 0xf0*4，元素为 4 字节指针，索引为 type。
//   Ctor(D1) @ 0x08374ef4：在 +0x3e0 处构造 CGeneratorCommonData
//       （0x3e0 = 0x3c0 + 32 = 8 个生成器指针之后）
//   => WongWork::CItemGeneratorMgr 布局：
//          +0x000 CItemGenerator* m_generators[8]      （基址 +0x3c0，索引 type）
//          +0x3e0 CGeneratorCommonData m_common
//
// 注：ORIG 类位于 namespace WongWork（非全局）。本头为权威声明；GameStubs.cpp /
// CDataManager.h 中既存的【全局】CItemGeneratorMgr 是不同类型（不同 N 空间），
// 与本头无符号/ODR 冲突。
// ============================================================================
#ifndef GAME_CITEMGENERATORMGR_H_
#define GAME_CITEMGENERATORMGR_H_

namespace WongWork
{

class CItemGenerator;  // 前置：生成器对象（多项生成具体类派生自它）

class CItemGeneratorMgr
{
public:
    enum eGenerateType_t
    {
        eGenerateType_0 = 0,
        eGenerateType_1 = 1,
        eGenerateType_2 = 2,
        eGenerateType_3 = 3,
        eGenerateType_4 = 4,
        eGenerateType_5 = 5,
        eGenerateType_6 = 6,
        eGenerateType_7 = 7
    };

    // ORIG 0x0814a7d0：返回 m_generators[type]（字节偏移 0x3c0 + type*4）
    CItemGenerator* getGenerator(eGenerateType_t type);

    ~CItemGeneratorMgr();  // 定义于 CDataManagerScripts.cpp
    void setDropRatio(float ratio);  // 定义于 CDataManagerScripts.cpp（逐生成器下发）

    CItemGenerator* m_generators[8];  // +0x000（ORIG 基址 +0x3c0 = 0xf0*4 字节）
};

}  // namespace WongWork

#endif  // GAME_CITEMGENERATORMGR_H_