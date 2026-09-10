int BuyUpgradeData::getStatFirstPos() const
{
    // b28d-35: DIFF→N 封顶（P197④ 镜像族：ORIG @0x088a2f7a 对 call 结果算术
    //   比较出直跳 `ja`，4.4 恒物化 `setbe;test;je`）。七形实测（/tmp/d35_sfp*.cpp
    //   h1-h21）：裸指针解引用直跳、迭代器 operator* 结果恒物化；< 0xfU 与 <= 0xeU
    //   同折；goto/取反/双比较（双比较需重复调用 operator*，违 P48）均不闭合。
    // b28c-L5：ORIG 0x088a2f7a 两处形状约束——
    //   判词（N197④ 反向·setcc 物化不可免）：ORIG 边界判定段为
    //     `mov (%eax),%eax / sub $0xa,%eax / cmp $0xe,%eax / ja <continue>`（4 条直跳）；
    //   4.4-O0 对「**寄存器内算术结果** vs 立即数」的比较且该 bool 有分支消费者时，
    //   恒物化 `setbe %al; test %al,%al; je`（5 条）。`< 0xfU` 与 `<= 0xeU` 两写法
    //   实测产物完全相同（同一 fold 路径）⇒ 非源形可控，属 P197④ 的镜像案例
    //   （该条记的是「4.4 恒直跳、ORIG 恒 sete」，此处相反）。语义与边界值全等
    //   （[FT_HP=0xa, FT_ETC2=0x18] 闭区间），判 setcc-fold 负例封顶。
    //   ① 取值段 `call de; mov (%eax),%eax; sub $0xa,%eax; cmp $0xe,%eax; ja`
    //      无槽往返 ⇒ *it 是表达式直用（P0），原实现具名 `const unsigned int ft`
    //      多出 `mov %eax,-0xc / mov -0xc,%eax` 一对并把 pos 槽顶到 -0x10；
    //   ② 增量段 `addl $1,-0xc(pos)` 后 `ppEi`（后置 ++，独立 sret -0x10）⇒
    //      pos 先自增 + it 后置。两处同改后 pos 回落 -0xc、三槽阶梯复原。
    int pos = 0;
    for (std::vector<FieldType::T>::const_iterator it = m_fieldTypes.begin();
         it != m_fieldTypes.end(); it++)
    {
        // b28d-solo-advancealtar_misc：ORIG 判定段 `sub $0xa,%eax / cmp $0xe,%eax / ja`
        //   ——**直跳**（无 bool 物化）。4.4 对 if 形（`< 0xfU`/`<= 0xeU`/`> 0xeU`
        //   加 continue/空 then 共七形实测）恒物化 `setbe;test;je`（+2 条，P197④ 镜像族）；
        //   改 15 个连续 case 汇聚到同一 `return pos` 的 switch 后，4.4 折叠为范围检查
        //   `sub/cmp/ja`，与 ORIG 逐条全等（探针 /tmp/sfp_p2.cpp fH vs fI）。
        //   语义 = *it ∈ [FT_HP(0xa), FT_ETC2(0x18)] 闭区间，与 if 形等价。
        switch ((unsigned int)*it - 10U)
        {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6:
        case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14:
            return pos;
        default:
            break;
        }
        ++pos;
    }
    return -1;
