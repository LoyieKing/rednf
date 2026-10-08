# -*- coding: utf-8 -*-
"""行情统计：从 SQLite（listing_obs / deals）聚合 K 线、箱线、成交量、均价、分位数。"""
import math
import time


def wquantile(pairs, q):
    """pairs: [(value, weight)]，加权分位数。"""
    pairs = sorted((v, w) for v, w in pairs if w > 0)
    if not pairs:
        return None
    tot = sum(w for _, w in pairs)
    target = q * tot
    acc = 0
    for v, w in pairs:
        acc += w
        if acc >= target:
            return v
    return pairs[-1][0]


def five(pairs):
    if not pairs:
        return None
    return {"min": min(v for v, _ in pairs), "p25": wquantile(pairs, 0.25), "p50": wquantile(pairs, 0.5),
            "p75": wquantile(pairs, 0.75), "max": max(v for v, _ in pairs),
            "mean": sum(v * w for v, w in pairs) / max(1, sum(w for _, w in pairs))}


def series(store, item_id, bucket=3600, days=7):
    now = time.time()
    since = now - days * 86400
    bucket = max(60, int(bucket))
    obs = store.query("SELECT ts, auction_id, is_bot, unit_price, count FROM listing_obs WHERE item_id=? AND ts>=? "
                      "ORDER BY ts", (item_id, since))
    deals = store.query("SELECT ts, count, unit_price, total, side, auction_id FROM deals WHERE item_id=? AND ts>=? "
                        "ORDER BY ts", (item_id, since))
    # 每次扫描
    scans = {}
    for o in obs:
        scans.setdefault(o["ts"], []).append(o)
    scan_rows = []
    for ts in sorted(scans):
        ls = scans[ts]
        scan_rows.append({"ts": ts, "min": min(l["unit_price"] for l in ls),
                          "supply": sum(l["count"] for l in ls),
                          "bot_supply": sum(l["count"] for l in ls if l["is_bot"]),
                          "listings": len(ls)})
    # 分桶
    B = {}

    def bk(ts):
        k = int(ts // bucket * bucket)
        if k not in B:
            B[k] = {"t": k, "ask_last": {}, "scan_mins": [], "supply": [], "deals": []}
        return B[k]
    for o in obs:
        b = bk(o["ts"])
        b["ask_last"][o["auction_id"]] = (o["unit_price"], o["count"])
    for s in scan_rows:
        b = bk(s["ts"])
        b["scan_mins"].append(s["min"])
        b["supply"].append(s["supply"])
    for d in deals:
        bk(d["ts"])["deals"].append(d)
    out = []
    for k in sorted(B):
        b = B[k]
        asks = list(b["ask_last"].values())
        row = {"t": k, "ask_box": five(asks)}
        if b["scan_mins"]:
            m = b["scan_mins"]
            row["ask_ohlc"] = {"o": m[0], "h": max(m), "l": min(m), "c": m[-1]}
            row["supply"] = b["supply"][-1]
        ds = b["deals"]
        if ds:
            pr = [d["unit_price"] for d in ds]
            vol = sum(d["count"] for d in ds)
            row["deal_ohlc"] = {"o": pr[0], "h": max(pr), "l": min(pr), "c": pr[-1]}
            row["volume"] = vol
            row["turnover"] = sum(d["total"] for d in ds)
            row["vwap"] = row["turnover"] / vol if vol else None
            row["deal_box"] = five([(d["unit_price"], d["count"]) for d in ds])
        else:
            row["volume"] = 0
        out.append(row)
    # 汇总
    dp = [(d["unit_price"], d["count"]) for d in deals]
    vol = sum(w for _, w in dp)
    summ = {
        "deals": len(deals), "volume": vol,
        "vwap": (sum(d["total"] for d in deals) / vol) if vol else None,
        "avg": (sum(v for v, _ in dp) / len(dp)) if dp else None,
        "q": {str(q): wquantile(dp, q / 100.0) for q in (5, 10, 25, 50, 75, 90, 95)} if dp else {},
        "by_side": {},
    }
    for d in deals:
        s = summ["by_side"].setdefault(d["side"], {"deals": 0, "volume": 0, "turnover": 0})
        s["deals"] += 1
        s["volume"] += d["count"]
        s["turnover"] += d["total"]
    last = scan_rows[-1] if scan_rows else None
    cur = None
    if last:
        ls = scans[last["ts"]]
        pairs = [(l["unit_price"], l["count"]) for l in ls]
        cur = {"ts": last["ts"], "min": last["min"], "supply": last["supply"], "bot_supply": last["bot_supply"],
               "listings": last["listings"], "box": five(pairs),
               "q": {str(q): wquantile(pairs, q / 100.0) for q in (10, 25, 50, 75, 90)}}
    return {"item_id": item_id, "bucket": bucket, "days": days, "buckets": out, "scans": scan_rows[-2000:],
            "deals_list": deals[-200:], "summary": summ, "current": cur}


def overview(store, days=1):
    since = time.time() - days * 86400
    return store.query("SELECT item_id, COUNT(*) deals, SUM(count) volume, SUM(total) turnover, "
                       "MIN(unit_price) low, MAX(unit_price) high FROM deals WHERE ts>=? GROUP BY item_id "
                       "ORDER BY turnover DESC", (since,))
