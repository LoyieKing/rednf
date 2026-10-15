// ----------------------------------------------------------------------------
// ORIG T 0x08349da0 _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
// （size 0x1ab）头像随机掉落主逻辑：
//   ① 入门阈值：threshold = (int)((double)m_field00 * T_A[(u8)rarity]
//                                 * T_B[(u8)grade - 1])；
//     T_A 基址 info+0x18（flds 0x8(%eax,%edx,4)，edx=(u8)rarity+4）、T_B 基址
//     info+0x8、下标 (u8)grade-1 —— 两片均为 clear 清零区浮点表的重叠视图，
//     按指针视图逐指令转写；fistpll 截断取低 dword。
//     get_rand_int(100000) > threshold 时直接返回。
//   ② 共 rounds = *(unsigned*)(info+0x04)（无符号 seta 比较）轮：
//       每轮 r = get_rand_int(*(int*)(info+0x2c))；
//       自 m_elements.begin() 重新遍历：首个 elem.m_field04 > r（无符号）时，
//       Inven_Item{}；item.m_addInfo(ORIG 写 +0x02 视图) ← elem.m_field00；
//       G_CDataManager()->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)1,
//       item, 1)；out.push_back(item)；命中后不推进迭代器即退出本轮
//       （ORIG jmp +0x170 → 轮数 +1 继续）。
// [b28d-L14 N18a] ORIG 0x08598f34；p167 缺 1 次 const_iterator ptEv；残差=槽深/寄存器。
// [F2e-Singles2 C05② 2026-09-10] 旋钮①删 rounds 具名局部（P0：ORIG 尾测每轮重读
//   `mov 0x8(%ebp),%eax; mov 0x4(%eax),%eax; cmp -0x10(%ebp),%eax; seta`，而我方缓存
//   rounds 于 -0x14 并折 `setb`）⇒ 尾测与 threshold 槽(-0x18→-0x14)一次对齐 ORIG；
//   旋钮②base/tableA/tableB 引用内联 + longlong 整数域（fildll/fistpll）⇒ thresholdLL。
//   实测 our 129→121（ORIG 122，gap **−7→−1**），TU 四数 27/38/0/2 两态逐位一致。
//   残余 = 4.4 寻址折叠（我方 `add $0x6; shl $0x2; add; flds (%eax)` vs ORIG
//   `add $0x4,%edx; flds 0x8(%eax,%edx,4)`，复现需 .h 成员数组视图，本任务禁改 .h；
//   N125 族）+ 槽号差（N82）。
void generateDropAvatarItem(const stEnamyDropAvatarInfo_t& info,
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out)
{
    // ① 入门阈值（ORIG fildll/flds/fmulp/fistpll 序列：64 位整数域 + float 乘法，
    //    表引用与 base 指针内联 —— ORIG 每访问点 `mov 0x8(%ebp),%eax` 重读参数，
    //    无 base/tableA/tableB 具名槽）。
    long long thresholdLL =
        static_cast<long long>(static_cast<unsigned int>(info.m_field00));
    // [Q7S5 2026-09-11 形态轴·triage] ORIG 两处表访问为成员数组寻址形
    //   0x08349dd2/0x08349de2 `add $0x4,%edx; flds 0x8(%eax,%edx,4)` /
    //   `lea -0x1(%eax),%edx; flds 0x8(%eax,%edx,4)`；旧稿用
    //   `reinterpret_cast<char*>(&info)+0x8+4*(…)` 指针算术 ⇒ 4.4 折成
    //   `add $0x6; shl $0x2; add; flds (%eax)`（N125 族）。改为 **既有视图成员**
    //   `m_field08[i]`（STMonsterScript.h:+0x08 float[9]，与 union 的
    //   m_field08v 同址）—— **地址与语义逐值不变**，仅寻址形态对齐 ORIG。
    thresholdLL = static_cast<long long>(
        static_cast<float>(thresholdLL) *
        info.m_field08[static_cast<unsigned>(rarity) + 4] *
        info.m_field08[static_cast<unsigned>(grade) - 1]);
    const int threshold = static_cast<int>(thresholdLL);
    if (get_rand_int(100000) > threshold)
        return;

    // ② roll 轮次
    for (unsigned int round = 0; round < (unsigned int)info.m_field04; ++round)
    {
        const int r = get_rand_int(info.m_field2c);
        std::vector<stEnamyDropAvatarElement_t>::const_iterator it =
            info.m_elements.begin();
        while (it != info.m_elements.end())
        {
            if (static_cast<unsigned int>(it->m_field4) >
                static_cast<unsigned int>(r))
            {
                Inven_Item item;
                item.m_addInfo = it->m_field0;              // ORIG 写 +0x02 视图
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)1, item, 1);
                // [Q7S5 2026-09-11 语义轴] ORIG 0x08349ec0（create_item 之后、
                //   push_back 之前）：`ptEv; movzbl 0x8(%eax),%eax; movzbl %al,%eax;
                //   mov %ax,-0x52(%ebp)` —— -0x52(%ebp) = &item(-0x5d)+0xb = Inven_Item
                //   ::m_endurance(endurance, ushort) ⇒ 原码此处把 elem.m_field8 写入
                //   item 耐久字段；缺失 ⇒ 掉落头像物品 m_endurance 保持 create_item 后
                //   的 0，玩家可见耐久为空。p167 该处缺的 1 次 const_iterator::ptEv
                //   亦由本行补回。
                item.m_endurance = it->m_field8;
                out.push_back(item);
                break;                                      // 不推进迭代器（ORIG jmp +0x170）
            }
            it++;
        }
    }
}