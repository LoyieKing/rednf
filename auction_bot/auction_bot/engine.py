# -*- coding: utf-8 -*-
"""
经济模型引擎：把运营 agent 校准的代理人模型（/workspace/ops/econ/sim.py + model_config.json）
拆成“每步一次”的函数，在机器人里按真实时间驱动。

每一步（5~15 分钟随机）对每个建模道具：
  1. 世界库存：产出 faucet、消耗 sink（随在线人数曲线缩放）
  2. 公允价：fair = (c + (v-c)·稀缺度)·e^x·price_mult，x 为 OU 噪声
  3. 卖方代理（生产者 / 抛售者 / 倒爷）→ 新的系统挂单（owner -1）
  4. 买方代理（消费者 / 倒爷）在真实挂单簿上撮合：玩家挂单优先，其次系统挂单
输出“计划”（新挂单、成交），由 bot.py 通过拍卖服协议执行：
  * 成交系统挂单 → 假买家 1999999999 BUY_ITEM_APIECE（同一挂单同一步合并为一次购买）
  * 成交玩家挂单 → 真实购买（受每日金币上限等护栏约束）
与 sim.py 的差异仅在于：时间步长取真实间隔（dt），在线人数按本地时钟（小时、周末），
挂单簿来自拍卖服实时检索，另加 volume_scale 总量系数和过期补挂。
"""
import math
import random
import time


def clamp(x, lo, hi):
    return max(lo, min(hi, x))


def round_price(px):
    """与 sim.py 相同：按价位取整，让挂单价格看起来像真人报价。"""
    if px < 100: return max(1, int(round(px)))
    if px < 1000: return int(round(px / 5) * 5)
    if px < 10000: return int(round(px / 10) * 10)
    if px < 100000: return int(round(px / 50) * 50)
    if px < 1000000: return int(round(px / 500) * 500)
    return int(round(px / 5000) * 5000)


def poisson(lam, rng):
    if lam <= 0: return 0
    if lam > 30:
        return max(0, int(round(rng.gauss(lam, math.sqrt(lam)))))
    L, k, p = math.exp(-lam), 0, 1.0
    while True:
        p *= rng.random()
        if p <= L: return k
        k += 1


def lot(q, rng):
    v = q['median'] * math.exp(rng.gauss(0, q.get('sigma', 0.5)))
    return int(clamp(round(v), q.get('min', 1), q.get('max', 999)))


def online_now(g, ts=None):
    """与 sim.online_at 同一曲线，但按本地真实时间（小时 + 周六日）。"""
    lt = time.localtime(ts or time.time())
    hour = lt.tm_hour + lt.tm_min / 60.0
    oc = g['online_curve']
    on = oc['base'] + oc['amp'] * max(0.0, math.sin((hour - 8) / 24 * math.tau)) ** oc['power']
    if lt.tm_wday in (5, 6):
        on *= oc['weekend_mult']
    return int(on)


def fair_price(p, st, g):
    sc = g['scarcity']
    F = max(p['faucet_per_h'], 1e-9)
    cover_h = st['stock'] / F
    scar = clamp(sc['a'] - math.log1p(cover_h) / sc['b'], sc['lo'], sc['hi'])
    base = p['c'] + (p['v'] - p['c']) * scar
    return base * math.exp(st['x']) * p.get('price_mult', 1.0), scar


class Engine:
    def __init__(self, model, items, ecfg, state=None, rng=None):
        """model: model_config.json；items: {iid(int): 参数}（已过滤排除项/白名单/覆盖）；state: 持久化状态。"""
        self.g = model['global']
        self.items = items
        self.ecfg = ecfg
        self.rng = rng or random.Random()
        self.state = state or {}
        for iid, p in items.items():
            k = str(iid)
            st = self.state.get(k)
            if st is None:
                self.state[k] = {"stock": p['faucet_per_h'] / max(p['sink_rate_per_h'], 1e-9),
                                 "x": self.rng.gauss(0, p['ou_sd'] * 0.5), "last": p['ref'], "ref": p['ref']}
            elif st.get("ref") != p['ref']:
                # 模型更新改了价格锚点（如神秘商店道具）：重置价格状态，库存按新产出/消耗重新取均衡值
                if st.get("ref") is not None or abs(st.get("last", p["ref"]) / p["ref"] - 1) > 0.25:
                    st.update({"stock": p['faucet_per_h'] / max(p['sink_rate_per_h'], 1e-9),
                               "x": self.rng.gauss(0, p['ou_sd'] * 0.5), "last": p['ref']})
                st["ref"] = p['ref']

    def fair(self, iid):
        p = self.items[iid]
        return fair_price(p, self.state[str(iid)], self.g)

    def step(self, dt_h, book, expired=None, now=None):
        """
        book: {iid: [ask]}，ask = {auction_id, price(单价), qty, owner_id(-1=系统), seller, buyable(bool)}
        expired: {iid: [剩余数量,...]}  上一步以来到期下架、仍有剩余的系统挂单（用于补挂）
        返回 plan: {"online", "items": {iid: {...}}, "new": [...], "fills": [...]}
        """
        g = self.g
        rng = self.rng
        dt_h = clamp(dt_h, 1 / 60.0, float(self.ecfg.get("max_dt_minutes", 30)) / 60.0)
        dt_d = dt_h / 24.0
        online = online_now(g, now)
        k = online / 800.0
        vs = float(self.ecfg.get("volume_scale", 1.0))
        sh, ap = g['agent_shares'], g['agent_act_prob']
        plan = {"online": online, "dt_h": dt_h, "items": {}, "new": [], "fills": []}
        for iid, p in self.items.items():
            st = self.state[str(iid)]
            # 1) 世界库存
            Fd = p.get('drop_faucet_per_h', p['faucet_per_h'])   # 掉落等常规产出（神秘商店产出单独模拟）
            prod = max(0.0, rng.gauss(Fd * dt_h, 0.15 * Fd * dt_h)) * k
            cons = st['stock'] * p['sink_rate_per_h'] * dt_h * k
            st['stock'] = max(0.0, st['stock'] + prod - cons)
            # 2) OU 噪声
            st['x'] += -p['kappa_per_day'] * st['x'] * dt_d + p['sigma_per_day'] * math.sqrt(dt_d) * rng.gauss(0, 1)
            fair, scar = fair_price(p, st, g)
            ref = st['last'] if st['last'] > 0 else fair
            asks = [dict(a) for a in book.get(iid, [])]
            A = g['activity_scale'] * vs * p['activity'] * dt_h
            n_sys = sum(1 for a in asks if a['owner_id'] == -1)
            cap = int(p['target_listings'] * 2 * float(self.ecfg.get("listing_cap_scale", 1.0)))
            info = {"fair": fair, "scarcity": scar, "stock": st['stock'], "x": st['x'], "online": online,
                    "new": 0, "virtual_qty": 0, "user_qty": 0, "virtual_trades": 0, "user_trades": 0}
            new = []

            def add_new(px, qty, seller):
                a = {"auction_id": None, "price": round_price(px), "qty": qty, "owner_id": -1, "seller": seller,
                     "buyable": True, "new_idx": len(plan["new"]) + len(new)}
                new.append({"item_id": iid, "price": a["price"], "qty": qty, "seller": seller})
                asks.append(a)
            # 0) 过期补挂：剩余数量按当前公允价附近重新挂出（只补到 target_listings）
            for rem in (expired or {}).get(iid, []):
                if n_sys >= p['target_listings'] or rem <= 0:
                    break
                add_new(fair * math.exp(rng.gauss(0.02, 0.05)), rem, "relist"); n_sys += 1
            # 3) 卖方：生产者
            for _ in range(poisson(online * sh['producer'] * ap['producer'] * A, rng)):
                if n_sys >= cap: break
                margin = 0.02 + 0.10 * max(0.0, ref / fair - 1.0)
                px = fair * math.exp(rng.gauss(margin, 0.05))
                px = max(px, p['c'] * p.get('price_mult', 1.0) * 0.9)
                add_new(px, lot(p['lot_qty'], rng), "producer"); n_sys += 1
            # 神秘商店（材料商人 NPC 1003）：通关时随机出现，基价×{0.9,1.0,1.1}，每人限购 limit 个。
            # 市价不低于商店价时玩家买满（计入世界库存）；市价比商店价高 8% 以上时，一部分人转手挂到拍卖行（价格软上限）。
            ss = p.get('secret_shop')
            if ss:
                lam = k * g.get('runs_market_per_h', 240.0) * ss['p_offer_per_run'] * dt_h
                for _ in range(poisson(lam, rng)):
                    roll = rng.choice(ss['price_rolls'])
                    if ref < roll:
                        continue
                    q = int(ss['limit_per_visit'])
                    st['stock'] += q
                    info["shop_visits"] = info.get("shop_visits", 0) + 1
                    if ref > roll * 1.08 and n_sys < cap and rng.random() < ss.get('resale_prob', 0.5):
                        add_new(roll * rng.uniform(1.05, 1.15), q, "secret_shop_resale"); n_sys += 1
            # 抛售者
            for _ in range(poisson(online * sh['dumper'] * ap['dumper'] * A, rng)):
                if n_sys >= cap: break
                q = dict(p['lot_qty']); q['median'] = q['median'] * 2
                add_new(ref * rng.uniform(0.84, 0.95), lot(q, rng), "dumper"); n_sys += 1
            fills = {}

            def buy(qty, reserve, buyer):
                bought = 0
                cand = [a for a in asks if a['qty'] > 0 and a['buyable'] and a['price'] <= reserve]
                users = sorted([a for a in cand if a['owner_id'] != -1], key=lambda a: a['price'])
                syss = sorted([a for a in cand if a['owner_id'] == -1], key=lambda a: a['price'])
                for a in (users + syss if g.get('user_priority', True) else sorted(cand, key=lambda a: a['price'])):
                    if bought >= qty: break
                    q = min(qty - bought, a['qty'])
                    a['qty'] -= q; bought += q
                    st['last'] = a['price']
                    key = ("aid", a['auction_id']) if a['auction_id'] else ("new", a['new_idx'])
                    f = fills.setdefault(key, {"item_id": iid, "auction_id": a['auction_id'], "new_idx": a.get('new_idx'),
                                               "unit_price": a['price'], "qty": 0, "user": a['owner_id'] != -1,
                                               "owner_id": a['owner_id'], "owner_name": a.get('owner_name'),
                                               "buyers": {}})
                    f["qty"] += q
                    f["buyers"][buyer] = f["buyers"].get(buyer, 0) + q
                    if a['owner_id'] != -1:
                        st['stock'] += q          # 玩家卖给虚拟世界 → 世界库存增加
                        info["user_qty"] += q; info["user_trades"] += 1
                    else:
                        info["virtual_qty"] += q; info["virtual_trades"] += 1
                return bought
            eps = p['epsilon']
            for _ in range(poisson(online * sh['consumer'] * ap['consumer'] * A, rng)):
                reserve = fair * (1.0 + 0.10 / eps) * math.exp(rng.gauss(0, 0.06))
                buy(lot(p['consumer_qty'], rng), reserve, 'consumer')
            # 5) 倒爷
            for _ in range(poisson(online * sh['flipper'] * ap['flipper'] * A, rng)):
                best = min((a['price'] for a in asks if a['qty'] > 0 and a['buyable']), default=None)
                if best is not None and best < fair * 0.96:
                    q = buy(rng.randint(1, max(1, p['consumer_qty']['max'])), fair * 0.96, 'flipper')
                    if q > 0 and n_sys < cap:
                        add_new(fair * rng.uniform(1.0, 1.04), q, "flipper"); n_sys += 1
            info["new"] = len(new)
            info["n_sys"] = n_sys
            plan["items"][iid] = info
            plan["new"].extend(new)
            plan["fills"].extend(fills.values())
        return plan
