void HistoryLog::buyItem(AdvanceAltarShopType::T shopType, int a, short b,
                         short c, int d)
{
    HistoryLog::advanceAltarstatistics_.incrementLayerStatData(
        static_cast<unsigned short>(shopType), static_cast<unsigned int>(a),
        static_cast<unsigned short>(c),
        static_cast<unsigned short>(1) /* useCountDelta 固定 1 */,
        static_cast<unsigned short>(d));
