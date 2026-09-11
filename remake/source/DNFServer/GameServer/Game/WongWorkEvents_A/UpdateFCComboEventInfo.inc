// ----------------------------------------------------------------------------
// EventFCCombo::UpdateFCComboEventInfo —— ORIG T 0x083fb929 (size 0x4d)
// ----------------------------------------------------------------------------
void EventFCCombo::UpdateFCComboEventInfo(unsigned int id, int itemNo)
{
    MySQL* db = GetDBHandle();
    db->set_query("upDate event_combo_entry set item_check=unix_timestamp(now()) where id=%s",
                  NumberToString(id, 0));
    db->exec(true);
}