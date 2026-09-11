// ----------------------------------------------------------------------------
// ORIG T 0x08133e30（updateStatistics.md 全文）：CAdvanceAltarLog::updateDatabase
 // (advanceAltarstatistics_)。依赖随 CAdvanceAltarLog 批次解锁。
// ----------------------------------------------------------------------------
void HistoryLog::updateStatistics()
{
    HistoryLog::advanceAltarstatistics_.updateDatabase();
}