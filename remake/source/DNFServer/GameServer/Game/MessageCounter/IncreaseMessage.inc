// ORIG IncreaseMessage @0x0856f9c4：命中则 ++count → 64 位累加耗时字节 →
// 滑动平均 avg = avg*((n'-1)/n') + newBytes*(1/n')；未命中插入首记录。
// [R29-1 按 ORIG 报告逐条重写] 六项：
// ① `it->second` 逐次重读（ORIG 6× _Rb_tree_iterator::ptEv，原我方具名引用 `value`
//    只 1 次 ⇒ p167 MISSING 6）；
// ② 比较用正向 `!=`（ORIG `neq`；我方原 `==` 生 `eq` ⇒ EXTRA）；
// ③ bytes 累加为 64 位 `+=`（ORIG 0x0856fa8a `mov;mov;add;adc;mov;mov`；
//    原我方分 low/high + 进位判断多 1 槽 3 指令）；
// ④ `wOld`/`wNew` 两具名 double，声明于 find 之后（ORIG 预清 `fldz;fstpl` ×2 位于
//    find 之后 end 之前，槽 -0x18/-0x10，wOld 更深 ⇒ 先声明）；
// ⑤ 未命中插入记录 = { count=1, bytes=(unsigned long long)bytes,
//    avg=(double)bytes }（ORIG 0x0856fb3e `movl $0x1,-0x7c`、bytes→-0x78/-0x74、
//    `fildll -0x98; fstpl -0x70`）——**原我方写 `{0,0,0,0.0}` 是语义 bug**
//    （首记录丢 bytes/avg、count 从 0 起）；
// ⑥ 平均更新书写序 `m_avg * wOld + (double)bytes * wNew`（ORIG 首条 `fldl 0x14(%eax)`
//    即 avg ⇒ P243 左操作数先 fld）。
void CMessageCounter::IncreaseMessage(unsigned int id, unsigned char type,
                                      unsigned short size, long long bytes)
{
    MessageKey key((int)id, type, size);
    std::map<MessageKey, MessageValue>::iterator it = m_map.find(key);
    double wOld = 0.0;
    double wNew = 0.0;
    if (it != m_map.end())
    {
        ++it->second.m_count;
        // [N2-S5 负结论] ORIG 0x0856fa94 `add -0x98(%ebp),%eax; adc -0x94(%ebp),%edx`
        // 为「左值在 eax/edx + 内存源」形；`+=` 实测 4.4 反选「先加载右值
        // bytes 到 eax/edx」，且显式 `m_bytes = m_bytes + bytes` 形更劣
        // （132/139 → 132/141）⇒ 保持 `+=`；本处 2 条 + %esi 保活连带
        // （push/pop + esp 恢复 epilogue）归 4.4 regmove 差异。
        it->second.m_bytes += (unsigned long long)bytes;

        wOld = (double)(it->second.m_count - 1) / (double)it->second.m_count;
        wNew = 1.0 / (double)it->second.m_count;
        it->second.m_avg = it->second.m_avg * wOld + (double)bytes * wNew;
        return;
    }

    // 未命中：插入首记录（ORIG 0x0856fb3e 段）。左值实参 make_pair(key, zero)，
    // ORIG 发射 _ZSt9make_pairIRN15message_counter10MessageKeyERNS0_12MessageValueEE。
    MessageValue zero;
    zero.m_count = 1;
    zero.m_bytes = (unsigned long long)bytes;
    zero.m_avg = (double)bytes;
    m_map.insert(std::make_pair(key, zero));
}