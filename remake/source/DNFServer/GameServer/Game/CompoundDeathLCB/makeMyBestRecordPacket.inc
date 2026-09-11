// ORIG T 0x08468b8e。查 best 表：命中 put_byte(1)+纪录；否则 put_byte(0)。
// 【切片转写】0x08468b9e `call _getBestRecordTable`（不内联算址）；find/end 结果
// 各经 _Hashtable_iterator → _Hashtable_const_iterator **转换构造 2 次**
// （0x08468bc5/0x08468bfb），比较用 const_iterator::operator==、取值用
// const_iterator::operator-> ⇒ 必须声明 const_iterator 并写 `==`（iterator +
// `!=` 会走 operator!= 并省掉两次转换 ctor，实测差 5 call / 9 insn）。
// 【batch-12 标注】o=63 n=61（gap 9 → 2）：调用点差集 10/10 归零（残差仅
//   _Hashtable_const_iterator 转换 ctor 的 C2↔C1 符号名）；余 2 = 尾部 nop/装载
//   步序（§6.1/N82）⇒ N 封顶。
void CDeathTowerRanking::makeMyBestRecordPacket(unsigned int towerIdx,
                                                unsigned int key,
                                                PacketGuard& packet)
{
    // 【b28d-solo-CompoundDeathLCB 判词】NEAR（63/63 助记符逐条全等；仅 find/end
    // 两组的 sret 临时与迭代器槽号互换：ORIG find 临时 -0x28/it -0x30、
    // last -0x20/end 临时 -0x18；我们 it -0x28/find 临时 -0x20、end 临时 -0x18/
    // last -0x30）。修法：补具名引用局部 `const stBestRecord_t& rec = it->second;`
    // 复现 ORIG ptEv 后 `add $0x4` + 槽回读（63→63）。试过「先声明后赋值」形
    // ⇒ EXTRA `_Hashtable_const_iterator::C1Ev` ×2（P50 负例，已回退）；指针/引用
    // best、const/非 const 变量四形槽号不变 ⇒ N86/N93 封顶。
    BestMap* best = _getBestRecordTable(towerIdx);
    // [b28f-SweepGroup17 判据⑩ 实测负例] 试过交换为「先 end/last、后 find/it」以复现
    //   ORIG 0x08468b8e 槽阶梯（end 临时 -0x18 < last -0x20 < find 临时 -0x28 <
    //   it -0x30）⇒ NEAR→**DIFF**（near 2→1 / diff 3→4）反劣，已回滚。
    //   维持原序（find→end）封顶 N86/N93。
    const BestMap::const_iterator it = best->find(key);
    // [b28f-SweepGroup17 判据⑩ 配方①（L29）] ORIG 不给 end() 具名槽 ⇒ 内联 end()
    //   调用（不落 `last` 对象槽），只保留 it 具名 + end() 的 sret 临时。
    if (it == best->end())
    {
        packet.put_byte(0);
    }
    else
    {
        packet.put_byte(1);
        // ORIG 0x08468c42..c4a：ptEv 后 `add $0x4,%eax; mov %eax,-0xc(%ebp)`，
        // 再于实参处 `mov -0xc(%ebp),%eax` 回读 ⇒ 原码有具名引用局部（P0 槽往返）。
        const stBestRecord_t& rec = it->second;
        _makeRecordPacket(rec, packet);
    }
}