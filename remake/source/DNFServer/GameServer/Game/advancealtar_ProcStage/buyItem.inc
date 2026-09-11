// ----------------------------------------------------------------------------
// ORIG T 0x08133dda（buyItem.md 全文）：
 // incrementLayerStatData(advanceAltarstatistics_, shopType, param2, param4, 1,
 // param5)——param3(第 1 个 short) 未参与调用，ORIG 原样。
// ----------------------------------------------------------------------------
void HistoryLog::buyItem(AdvanceAltarShopType::T shopType, int a, short b,
                         short c, int d)
{
    HistoryLog::advanceAltarstatistics_.incrementLayerStatData(
        static_cast<unsigned short>(shopType), static_cast<unsigned int>(a),
        static_cast<unsigned short>(c),
        static_cast<unsigned short>(1) /* useCountDelta 固定 1 */,
        static_cast<unsigned short>(d));
}