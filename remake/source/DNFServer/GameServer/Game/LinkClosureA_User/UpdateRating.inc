// ---------------------------------------------------------------------------
// CUser::UpdateRating（ORIG W 0x08568e9c 报告缺失；objdump 六条 call 序忠实）:
// enableSaveCharacStat(); SetRating(rating);
// ---------------------------------------------------------------------------
void CUser::UpdateRating(float rating)
{
    enableSaveCharacStat();                                // [0x8568ea8]
    SetRating(rating);                                     // [0x8568eba SetRatingEf]
}