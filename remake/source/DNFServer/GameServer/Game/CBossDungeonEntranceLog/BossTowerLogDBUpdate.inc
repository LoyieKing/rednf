// ORIG 0x081568cc：size>0 时打包 header(0x2e0,-1,size) + 逐项(key + 0x10 字节
// value)，随后 clear 并投递 DB 队列（QUEUE_IDX 4）。
void CBossDungeonEntranceLog::BossTowerLogDBUpdate()
{
    // [round2-L30] 修法四则（ORIG 0x081568cc size 0x973）：
    //   ①函数头 begin+end 双求值（P13/P214④，ORIG 0x081568d4/0x081568e9）；
    //   ②删 `Stream* stream` 具名局部（Acquire 结果直传 ctor，P0/配方③）；
    //   ③3 处 header 写 + 1 处 key 写补 `guard.operator*()->operator<<(x)`（P132）；
    //   ④循环体 `it->first`（ptEv）+ 具名 key + `guard.operator->()->GetInBuffer`
    //      （P103/P0）。
    // 结果 116→126（orig 127，gap 11→1）；残余 = 槽号分区差（判据⑫ 封顶）：
    //   ORIG 浅→深 = out -0xc / count -0x10 / it -0x14 / itEnd -0x18 / guard -0x20 /
    //   key -0x24；我方 = out -0xc / key -0x10 / count -0x14 / it -0x18 / itEnd -0x1c /
    //   guard -0x24 ⇒ 4 个头部变量整体深 4（N126 版本首槽基址常数），而循环内具名
    //   `key` 方向相反（ORIG 占最深槽 vs 我方占浅槽）= 4.1/4.4 对「循环内标量 vs 头部
    //   聚合」的槽分区方向相反（P331 分区律）。实测：把 `unsigned int key;` 上提到
    //   函数头（先声明深）槽号**逐值不变**（19:44 实验，已回退）⇒ 无源码解，N126 封顶。
    // ORIG 0x081568d4：头部先 `call begin`（-0x14）再 `call end`（-0x18）——两个
    //   迭代器都在函数头求值（P13/P214④）。
    std::map<unsigned int, STBossDungeonTowerLog>::iterator it = m_logMap.begin();
    std::map<unsigned int, STBossDungeonTowerLog>::iterator itEnd = m_logMap.end();

    // ORIG Acquire 结果**无槽**（0x0815690b `mov %eax,0x4(%esp)` 直传 CStreamGuard
    //   C1）⇒ 删我方 `Stream* stream` 具名局部（P0/配方③）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localglobal/global_Statistics.cpp", 0x69),
        true);                                  // ORIG CStreamGuard(pSVar2, 1)

    int count = (int)m_logMap.size();          // ORIG map::size
    if (count > 0)
    {
        // P132：ORIG 0x0815694a..0x0815697x 每处 header 写入都是
        //   `lea -0x20(%ebp); call CStreamGuard::operator*(); …; call operator<<`
        //   ⇒ 源形必须保留 `guard.operator*()->operator<<(x)` 两层调用（直接
        //   `guard << x` 会省略 ORIG 的 de() 调用）。
        guard.operator*()->operator<<(0x2e0);  // DB 请求号
        guard.operator*()->operator<<((int)-1);
        guard.operator*()->operator<<(count);

        for (; it != itEnd; ++it)     // ORIG 用函数头缓存的 end（0x0815693d 段比较 -0x18）
        {
            // ORIG 0x0815699f..0x081569c6：`call _Rb_tree_iterator::ptEv()`（即
            //   `it->first`）→ 结果具名落槽 -0x24（P0：ORIG 有槽往返 ⇒ 原码有具名
            //   key）→ `%ebx` 跨 `CStreamGuard::de()` 保活 → operator<<(int)。
            unsigned int key = it->first;
            guard.operator*()->operator<<((int)key);
            // P103：ORIG 0x081569cb 先 `call CStreamGuard::ptEv()` 再调
            //   GetInBuffer ⇒ 源形必须保留 operator-> 两层链。
            STBossDungeonTowerLog* out =
                guard.operator->()->GetInBuffer<STBossDungeonTowerLog>();
            // [b28c-p167 真缺口] ORIG 0x081569e1 / 0x08156a06 两处均为
            //   `lea -0x14(%ebp); call _ZNKSt17_Rb_tree_iteratorISt4pairIKj21STBossDungeonTowerLogEEptEv`
            //   （即 operator->）再 `add $0x4,%eax` 取 value ⇒ 源形必须是
            //   `it->second`；写成 `(*it).second` 会发射 `deEv`（operator*）。
            //   p167 差集实证：MISSING ptEv ×2 / EXTRA deEv ×2（calls 27=27）。
            std::memcpy(out, &it->second, 0x10);
            std::memset(&it->second, 0, 0x10);
        }

        m_logMap.clear();                      // ORIG map::clear

        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
    }
}