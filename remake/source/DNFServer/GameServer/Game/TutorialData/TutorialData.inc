// ORIG _ZN12TutorialDataC1Ev @0x08ab7e4c：
// 容器按成员顺序构造（各数组循环展开），随后 *this=0，
// 最后对 avatarIndexMap 额外执行一次 clear()。
// [A31-GI · P327] 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
TutorialData::TutorialData()
{
    // 成员由编译器按声明顺序构造
    m_dungeonIndex = 0;
    m_avatarIndexMap.clear();
}