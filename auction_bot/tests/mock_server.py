# -*- coding: utf-8 -*-
"""
模拟 df_auction_r 的 GA 协议（按 remake 源码 HandlerFor_GA_.cpp / Auction.cpp 语义），用于离线测试。
严格模式：收到长度与源码定长不符、msg_no 越界、REGIST 重复等“可能危及真服”的包时记录违规。
"""
import asyncio
import struct
import time

from auction_bot import protocol as P

KNOWN_ITEMS = {3037: True, 3036: True, 3180: True, 1047: True, 3187: True}   # iteminfo.dat（True=可堆叠）
NPC_NAME = "拍卖行".encode("utf-8")


def ag(msg_no, body, ack=0):
    size = P.HEADER_SIZE + len(body)
    return struct.pack(P.HEADER_FMT, P.CAT_AG, msg_no, size, 0, 0, ack) + body


def pack_auction_item_info(l):
    b = struct.pack("<Qiii", l["auction_id"], l["price"], l["instant_price"], l.get("average", 0))
    name = l["owner_name"][:12]
    b += name + b"\0" * (13 - len(name))
    b += struct.pack("<d", 0.0) + bytes([2])
    b += P.pack_item_info(l["item_id"], add_info=l["count"])
    b += b"\0" * P.EMBLEM_SIZE + b"\0" * P.EXPANSION_SIZE
    b += struct.pack("<Ii", 0, l["unit_price"])
    assert len(b) == P.AUCTION_ITEM_INFO_SIZE
    return b


class MockAuction:
    def __init__(self):
        self.listings = {}
        self.next_id = 100
        self.violations = []
        self.received = []
        self.history = []
        self.ready = True

    async def handle(self, reader, writer):
        fr = P.FrameReader()
        registered = False
        try:
            while True:
                data = await reader.read(65536)
                if not data:
                    break
                for pkt in fr.feed(data):
                    h = P.parse_header(pkt)
                    self.received.append((h["cat"], h["msg_no"], len(pkt)))
                    if h["cat"] == P.CAT_HEARTBEAT:
                        continue
                    if h["cat"] != P.CAT_GA or h["msg_no"] not in P.GA_SIZES or P.GA_SIZES[h["msg_no"]] != len(pkt):
                        self.violations.append(("bad packet", h, len(pkt)))
                        continue
                    if h["msg_no"] == P.GA_REGIST:
                        if registered:
                            self.violations.append(("double REGIST", h))
                        registered = True
                        writer.write(ag(P.AG_REGIST_ACK, b"", ack=0))
                        continue
                    if not registered:
                        self.violations.append(("request before REGIST", h))
                    out = self.dispatch(h["msg_no"], pkt)
                    for o in out:
                        writer.write(o)
                await writer.drain()
        finally:
            writer.close()

    def dispatch(self, m, b):
        if m == P.GA_CHECK_AUCTION_READY:
            return [ag(P.AG_CHECK_AUCTION_READY, bytes([1 if self.ready else 0]))]
        if m == P.GA_ASK_REGISTED_ITEM_NUM:
            char_idx, cid = struct.unpack_from("<ii", b, 18)
            (item_id,) = struct.unpack_from("<I", b, 29)     # GA: item_id@29（AG 回包里在 @31）
            n = -2 if item_id not in KNOWN_ITEMS else sum(1 for l in self.listings.values() if l["owner"] == cid)
            body = struct.pack("<iihBHIiiii", char_idx, cid, n, 0, 0, item_id, 0, 0, 0, 0) + b"\0" * 12
            return [ag(P.AG_ASK_REGISTED_ITEM_NUM, body)]
        if m == P.GA_REGIST_ITEM:
            char_idx, owner = struct.unpack_from("<ii", b, 18)
            name = b[26:39].split(b"\0")[0]
            user_type, price, instant = struct.unpack_from("<Bii", b, 39)
            it = P.unpack_item_info(b, 48)
            (unit,) = struct.unpack_from("<i", b, 149)
            if 0 not in b[165:197] or 0 not in b[26:39]:
                self.violations.append(("owner_nexon_id not NUL terminated",))
            because = 0
            aid = 0
            if price == -1 and instant == -1:
                because = 0x1c
            elif instant != -1 and instant < price:
                because = 0x1c
            elif it["item_id"] not in KNOWN_ITEMS:
                because = 0                       # 源码：返回 0 但 auction_id = 0
            else:
                aid = self.next_id
                self.next_id += 1
                self.listings[aid] = {"auction_id": aid, "owner": owner,
                                      "owner_name": NPC_NAME if owner == -1 else name, "price": price,
                                      "instant_price": instant, "unit_price": unit, "item_id": it["item_id"],
                                      "count": it["add_info"], "expire": time.time() + 7200}
            log = struct.pack("<iBQiiIBiHIB", char_idx, 0 if not because else 1, aid, owner, price, it["item_id"], 0,
                              it["add_info"], 0, 0, because)
            res = struct.pack("<iiBB", char_idx, owner, 0 if because else 1, because)
            return [ag(P.AG_LOG_MESSAGE, log), ag(P.AG_REGIST_RESULT, res)]
        if m == P.GA_SEARCH_BY_ITEMKEY:
            char_idx, cid = struct.unpack_from("<ii", b, 18)
            start, us, ue, n, cat = struct.unpack_from("<IBBBH", b, 26)
            rs, re_ = struct.unpack_from("<BB", b, 47)
            ids = struct.unpack_from("<20I", b, 49)[:n]
            if n > 20 or ue > 31 or us > ue or re_ > 0x7f or rs > re_:
                return []                         # 源码：参数非法时不回包
            hits = [l for l in sorted(self.listings.values(), key=lambda x: x["auction_id"]) if l["item_id"] in ids]
            page = hits[start:start + 60]
            body = struct.pack("<iiBIH", char_idx, cid, 0, len(hits) if start == 0 else 0, len(page))
            body += b"".join(pack_auction_item_info(l) for l in page)
            return [ag(P.AG_ITEM_LIST, body)]
        if m == P.GA_BUY_ITEM_APIECE:
            char_idx, buyer, price, aid, cnt = struct.unpack_from("<iiiQi", b, 18)
            l = self.listings.get(aid)
            because = 0
            if not l:
                because = 0x0b
            elif l["price"] != -1 or price != l["unit_price"] * cnt or cnt > l["count"] or cnt <= 0:
                because = 0x1c
            else:
                l["count"] -= cnt
                l["instant_price"] = l["unit_price"] * l["count"]
                self.history.append(("apiece", aid, buyer, cnt))
                if l["count"] == 0:
                    del self.listings[aid]
            log = struct.pack("<iBQiiIBiHIB", char_idx, 4 if not because else 3, aid, 0, price, 0, 0, cnt, 0, 0, 0)
            res = struct.pack("<iiiBBQ", char_idx, buyer, price, 0 if because else 1, because, aid)
            return [ag(P.AG_LOG_MESSAGE, log), ag(P.AG_BUY_ITEM_APIECE, res)]
        if m == P.GA_BIDDING:
            char_idx, buyer = struct.unpack_from("<ii", b, 18)
            price, aid = struct.unpack_from("<iQ", b, 39)
            l = self.listings.get(aid)
            because = 0 if (l and price == l["instant_price"]) else 0x1c
            if not because:
                del self.listings[aid]
            log = struct.pack("<iBQiiIBiHIB", char_idx, 2, aid, 0, price, 0, 0, 0, 0, 0, 0)
            res = struct.pack("<iiiBBQ", char_idx, buyer, price, 0 if because else 1, because, aid)
            return [ag(P.AG_LOG_MESSAGE, log), ag(P.AG_BIDDING_RESULT, res)]
        return []


async def start(port=0):
    m = MockAuction()
    srv = await asyncio.start_server(m.handle, "127.0.0.1", port)
    return m, srv, srv.sockets[0].getsockname()[1]
