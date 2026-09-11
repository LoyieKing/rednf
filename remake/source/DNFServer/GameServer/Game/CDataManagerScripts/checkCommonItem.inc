// ORIG checkCommonItem 0x8918cdc：遍历 m_vec0 找 itemIdx
// [b28d-solo-CDataManagerScripts-2] DIFF→：原判「dead-bool XOR 哨兵非源码可表达」
// 被探针推翻——ORIG 的 `bool b` 哨兵在 4.4 下逐条可复现（/tmp/probe_cdms9.cpp k2：
// 帧 0x38、入口 `cmpb $0,-0xd; jne ret0`、槽 -0xd/-0x18/-0x1c/-0xc/-0x14 与 ORIG 全等）。
// 关键形 = 空 then + else 主体（`if(b){}else{…}`），既得入口直跳又无 goto 尾岛。
// 语义等价：b 初值 false，仅在条件下 `^=1`（0→1）且内层置回 0 ⇒ 守卫恒真，
// 遍历与命中返 true 的原语义不变。ORIG 0x08918cdc。
bool stBroadCastItemScript::checkCommonItem(int itemIdx) const
{
    bool b = false;
    if (b)
    {
        /* ORIG 空 then：守卫恒假，仅复现 4.1 的块布局 */
    }
    else
    {
        std::vector<int>::const_iterator it = m_vec0.begin();
        std::vector<int>::const_iterator e = m_vec0.end();
        for (; it != e && (b ^= 1, b); it++)
        {
            const int* p = &(*it);
            while (b)
            {
                if (*p == itemIdx)
                {
                    return true;
                }
                b = false;
            }
        }
    }
    return false;
}