// createRecipeInfo（自 GameStubs.cpp 迁移；ORIG 0x89ba83c）
// 语义：clear 后按 source 布局解析 3 组 pair 表 + 尾部 2 个标记字节 + 可选 extra 拷贝。
//   source = [count1, (p0,p1)xcount1, count2, (p0,p1)xcount2, count3, (p0,p1)xcount3,
//             tailValue?, ...]
//   m_vec0/count1、m_vecC/count2、m_vec18/count3；
//   若 source 仍有剩余元素：m_field24 = (tail==1)，m_field25 = (tail==2)（均读同一元素），
//   且 extra!=0 时 m_vec28 = *extra。
// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// [b28d-solo-CStackableItem] NEAR 225=225，本波残差 360→218 行（fndiff 归一化）。
// 新修法：①ORIG 三段 count 各占独立槽（0x89ba93e `mov %eax,-0x18`、第二段 -0x10、
// 第三段 -0xc）⇒ 源码 n1/n2/n3 三个具名局部（复用同一 n 时两侧都只给 1 槽）；
// ②n 独立化后帧自然 +0x10，原 P190「2 个取址 double framePad」必须删（留则帧 0x88
// 超 ORIG 0x78）⇒ 删后 n1=-0x18/i=-0x14 与 ORIG 逐槽一致、pair 临时 -0x50 亦对齐。
// [b28d-PoolSmall·P326 翻案] 旧残余判 N（it/end 两槽方向相反：ORIG it=-0x48(深)/
// end=-0x44(浅) = 4.1「先声明深」；4.4 恒「先声明浅」）已**归零**。三形并列读数：
//  ① 双具名（旧形）：it=-0x44 / end=-0x48 ⇒ 方向反，l8_cmp NDIFF 37；
//  ② P309「函数头声明翻向 + 逗号表达式保 clear→begin 序」：零效（仍 ①）；
//  ③ **P326 胜出形**：`end` 不具名、右值临时内联进比较 ⇒ begin 返回值直构到
//     具名 it=-0x48（深），end 的返回临时=-0x44（浅），逐槽与 ORIG 全等 ⇒ **NDIFF 0**
//     （227=227 逐条，p167 33=33 CLEAR）。end() 调用次数不变（临时构造仍 1 call），
//     故不破坏调用点差集；source 在本函数内无增删，迭代器不失效，语义等价。
//  ⇒ 与 P219 补①（翻求值序）/P236（find 内联实参）互斥，本件按 ORIG 槽深取 P326。
void createRecipeInfo(RecipeInfo* info, std::vector<int>* source,
                      std::vector<std::pair<int, int> >* extra)
{
    info->clear();
    // it/end 槽方向：本波 **P326** 归零（三形读数并列见本函数头判词）。
    std::vector<int>::iterator it = source->begin();
    if (it == source->end())
    {
        return;
    }
    // [b28d-solo-CStackableItem] ORIG 三个 count 局部各占独立槽
    // （0x89ba93e `mov %eax,-0x18`、第二段 `-0x10`、第三段 `-0xc`）⇒ 源码为
    // 三个不同具名局部（复用同一变量时 4.4/4.1 都只给一个槽）。
    int n1 = *it;
    it++;
    int i = 0;
    for (i = 0; i < n1; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec0.push_back(p);
    }
    int n2 = *it;
    it++;
    for (i = 0; i < n2; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vecC.push_back(p);
    }
    int n3 = *it;
    it++;
    for (i = 0; i < n3; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec18.push_back(p);
    }
    if (it == source->end())
    {
        return;
    }
    info->m_field24 = (*it == 1);
    info->m_field25 = (*it == 2);
    if (extra != 0)
    {
        info->m_vec28 = *extra;
    }
}