// ---------------------------------------------------------------------------
// 37. CUser::use_multibox_statistic @0x086767d2
// ---------------------------------------------------------------------------
void CUser::use_multibox_statistic(unsigned int itemIdx)
{
    // ORIG 0x086767d2：switch 线性下探 `mov param,%eax; cmp $K,%eax; je 体岛`×3，
    // 体岛按 0x25($0 fall)/0x2e($1)/0x37($2) 排布 = case 升序书写（技巧34/P46，
    // probe p5.g3 实证 4.4 逐条同形；此前 if-else-if 判 N18b 系误判）。
    int kind = 0;
    switch (itemIdx)
    {
        case 0x380:    kind = 0; break;
        case 0x28982d: kind = 1; break;
        case 0x289749: kind = 2; break;
        default: break;
    }

    SendRandomBoxStatistic(kind, (ENUM_RANDOMBOX_ACTION_KIND)1);                // ORIG 第二参恒 1
}