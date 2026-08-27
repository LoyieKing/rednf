#ifndef GAME_ARAD_RABBIT_BOX_H_
#define GAME_ARAD_RABBIT_BOX_H_

// ============================================================================
// df_game_r 还原 —— CRabbitBox（多态兔盒条目，ORIG vptr 写入 0x8b9da50）。
// 反汇编（df_game_r）：
//   C1 @0x08197e1e：+0x04=-1，+0x08=+0x04、+0x0c=+0x04、+0x10=+0x04（全部 -1）。
//   D1/D2 @0x08197e5a（虚析构，ORIG 含条件 delete 形态）、D0 @0x08197e80。
//   拷贝构造 @0x0852488c：逐 dword 复制 +0x04..+0x10。
//   operator= @0x08197ec2：同拷贝语义。
//   getItemIndex @0x08197e9e → +0x08；getItemCount @0x08197eaa → +0x0c。
//   setItemIndex @0x089c3338 → +0x08；SetItemCount @0x089c3346 → +0x0c；
//   SetDropRate @0x089c3354 → +0x10。+0x04 无 setter/getter（初值 -1 原样保留）。
// 全部生命周期符号为 ORIG W 弱符号 → 头内 inline 定义。
// ============================================================================

class CRabbitBox
{
public:
    CRabbitBox() : m_field4(-1), m_itemIndex(-1), m_itemCount(-1), m_dropRate(-1) {}
    virtual ~CRabbitBox() {}

    CRabbitBox(const CRabbitBox& other)
        : m_field4(other.m_field4), m_itemIndex(other.m_itemIndex),
          m_itemCount(other.m_itemCount), m_dropRate(other.m_dropRate) {}
    CRabbitBox& operator=(const CRabbitBox& other)
    {
        if (this != &other)
        {
            m_field4 = other.m_field4;
            m_itemIndex = other.m_itemIndex;
            m_itemCount = other.m_itemCount;
            m_dropRate = other.m_dropRate;
        }
        return *this;
    }

    int getItemIndex() const { return m_itemIndex; }       // 0x08197e9e
    int getItemCount() const { return m_itemCount; }       // 0x08197eaa

    void setItemIndex(int idx) { m_itemIndex = idx; }      // 0x089c3338
    void SetItemCount(int count) { m_itemCount = count; }  // 0x089c3346
    void SetDropRate(int rate) { m_dropRate = rate; }      // 0x089c3354

private:
    int m_field4;      // +0x04（ctor 置 -1，无访问器）
    int m_itemIndex;   // +0x08
    int m_itemCount;   // +0x0c
    int m_dropRate;    // +0x10
};

#endif  // GAME_ARAD_RABBIT_BOX_H_
