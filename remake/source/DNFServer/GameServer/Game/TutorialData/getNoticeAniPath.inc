// ORIG _ZN12TutorialData16getNoticeAniPathEi @0x08ab8c4c（sret）：
// 未命中复制空临时向量。
std::vector<TutorialData::TutorialNotice*> TutorialData::getNoticeAniPath(int key)
{
    std::vector<TutorialNotice*> empty;
    NoticeMap::iterator it = m_notices.find(key);
    // ORIG 0x08ab8cb3：`call ne; test %al,%al; je 0x87` ⇒ 源码用 != 正极性
    // （it!=end 走 fall 返回 it->second）；empty 分支 ORIG 0x08ab8cdf 调
    // vector::C1EOS4_（move 构造，GCC4.4.7 不隐式 move 局部返回值 ⇒ 显式
    // std::move，否则生成 C1ERKS4_ 拷贝 +5 条）。
    if (it != m_notices.end()) {
        return it->second;
    }
    // ORIG 0x08ab8cdf 调 vector::C1EOS4_（move 构造）。用 static_cast 到右值
    // 引用而非 std::move()——4.4.7 -O0 把 std::move 模板实例化为 out-of-line
    // call（多 1 条 call + 传参），ORIG 无该 call（4.1 内联该转换）。
    return static_cast<std::vector<TutorialNotice*>&&>(empty);
}