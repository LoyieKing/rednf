# -*- coding: utf-8 -*-
"""
GA/AG 协议编解码（游戏服 <-> 金币拍卖服 df_auction_r）。

权威来源（remake 源码，均为 pack(1) 布局，与原版 32 位二进制一致）：
  - nsl::PACKET_HEADER            ServerLab/ServerLib/basic_header/PacketDesign.h
  - GA 请求 / AG 回包结构         ServerLab/Auction/AuctionServer/auction_header/AuctionPacket.h
  - DnfItemInfo / ROI_Category    ServerLab/Auction/AuctionServer/GameData/auction/AuctionItem.h
  - 游戏侧发包长度（ctor setSize） Game/PCK_AUCTION_*_GA.h、Game/CAuction.cpp
  - 分发表 msg_no -> handler      auction_source/HandlerFor_GA_.h registFuncMap()

注意（拍卖服几乎不校验包）：
  * 收包端 RecvBuffer::Parse 只看 sLength：sLength < 0x12（尤其 0）会让解析死循环/错位，
    sLength 超过最大包长会断线。所以这里所有包长都写死为原版 ctor 的 setSize 值，并在
    encode 时断言实际字节数 == sLength。
  * msg_no 是有符号 char，>= 0x80 时会被符号扩展后越界查表，只允许 0..14。
  * check_sum / sequence / ack 收包端不读（ORIG RecvBuffer::Parse / TCPDispatcher::dispatch
    反汇编核对过），游戏侧也全部填 0（ctor memset）。这里同样填 0。
"""
import struct

HEADER_FMT = "<bbIIII"          # classification, msg_no, sLength, check_sum, sequence, ack
HEADER_SIZE = struct.calcsize(HEADER_FMT)
assert HEADER_SIZE == 0x12

CAT_GA = 0          # 游戏服 -> 拍卖服
CAT_AG = 1          # 拍卖服 -> 游戏服（金币）
CAT_PG = 0x13       # 点券拍卖回包（本进程不处理）
CAT_HEARTBEAT = -1  # 拍卖服每 30 秒发一次 (cat=-1,id=0,len=0x12)

# ---- GA 请求 msg_no（HandlerFor_GA_::registFuncMap）----
GA_REGIST = 0
GA_ASK_AVERAGE_PRICE = 1
GA_ASK_REGISTED_ITEM_NUM = 2
GA_REGIST_ITEM = 3
GA_REGIST_CANCEL = 4
GA_BIDDING = 5
GA_SEARCH_BY_ITEMKEY = 6
GA_SEARCH_BY_NOITEMKEY = 7
GA_MY_REGISTED_ITEM_INFO = 8
GA_MY_BIDDING_INFO = 9
GA_CHECK_AUCTION_READY = 13
GA_BUY_ITEM_APIECE = 14

# 每个 GA 包的固定长度（原版 ctor 的 memset/setSize）
GA_SIZES = {
    GA_REGIST: 0x16,
    GA_ASK_AVERAGE_PRICE: 0x2f,
    GA_ASK_REGISTED_ITEM_NUM: 0x3d,
    GA_REGIST_ITEM: 0xc5,
    GA_REGIST_CANCEL: 0x22,
    GA_BIDDING: 0x33,
    GA_SEARCH_BY_ITEMKEY: 0x81,
    GA_SEARCH_BY_NOITEMKEY: 0x33,
    GA_MY_REGISTED_ITEM_INFO: 0x1a,
    GA_MY_BIDDING_INFO: 0x1a,
    GA_CHECK_AUCTION_READY: 0x12,
    GA_BUY_ITEM_APIECE: 0x2a,
}

# ---- AG 回包 msg_no ----
AG_REGIST_ACK = 0x00
AG_SERVICE_UNAVAILABLE = 0x01
AG_ASK_AVERAGE_PRICE = 0x02
AG_ASK_REGISTED_ITEM_NUM = 0x03
AG_REGIST_RESULT = 0x04
AG_BIDDING_RESULT = 0x05
AG_REGIST_CANCEL_RESULT = 0x06
AG_ITEM_LIST = 0x07
AG_MY_REGISTED_ITEM_INFO = 0x08
AG_MY_BIDDING_INFO = 0x09
AG_LOG_MESSAGE = 0x0b
AG_SERVICE_AVAILABLE = 0x0c
AG_CHECK_AUCTION_READY = 0x0d
AG_BUY_ITEM_APIECE = 0x0e

AG_NAMES = {
    AG_REGIST_ACK: "REGIST_ACK",
    AG_SERVICE_UNAVAILABLE: "SERVICE_UNAVAILABLE",
    AG_ASK_AVERAGE_PRICE: "ASK_AVERAGE_PRICE",
    AG_ASK_REGISTED_ITEM_NUM: "ASK_REGISTED_ITEM_NUM",
    AG_REGIST_RESULT: "REGIST_RESULT",
    AG_BIDDING_RESULT: "BIDDING_RESULT",
    AG_REGIST_CANCEL_RESULT: "REGIST_CANCEL_RESULT",
    AG_ITEM_LIST: "ITEM_LIST",
    AG_MY_REGISTED_ITEM_INFO: "MY_REGISTED_ITEM_INFO",
    AG_MY_BIDDING_INFO: "MY_BIDDING_INFO",
    AG_LOG_MESSAGE: "LOG_MESSAGE",
    AG_SERVICE_AVAILABLE: "SERVICE_AVAILABLE",
    AG_CHECK_AUCTION_READY: "CHECK_AUCTION_READY",
    AG_BUY_ITEM_APIECE: "BUY_ITEM_APIECE",
}

# 拍卖服 TransErrToReason 的 result_because 码
REASON_TEXT = {
    0x96: "挂单不存在(0x24)",
    0x9a: "物品信息错误(0x2b)",
    0x9b: "不能购买自己的挂单(0x2d)",
    0x9c: "已有出价/状态不符(0x2e)",
    0x9d: "价格不符(0x2f)",
    0x91: "物品类型不可上架(0x23)",
    0x9e: "拍卖服数据库队列繁忙(0x31)",
    0x98: "价格非法(0x1f/0x26)",
    0x76: "低于均价(0x37)",
    0x99: "其他错误",
}

MAX_ITEM_ID_PER_SEARCH = 20   # Search::FindByItem: itemIdNum > 0x14 -> 0x1d
SEARCH_PAGE = 60              # IsFindEnough: numberOfFound > 0x3b
MAX_PACKET = 0x203d           # 最大回包（ITEM_LIST 满 60 条）


class ProtocolError(Exception):
    pass


def header(cat, msg_no, size):
    return struct.pack(HEADER_FMT, cat, msg_no, size, 0, 0, 0)


def parse_header(buf):
    cat, msg_no, size, cs, seq, ack = struct.unpack_from(HEADER_FMT, buf, 0)
    return {"cat": cat, "msg_no": msg_no, "size": size, "check_sum": cs, "sequence": seq, "ack": ack}


def _finish(msg_no, body):
    size = GA_SIZES[msg_no]
    pkt = header(CAT_GA, msg_no, size) + body
    if len(pkt) != size:
        raise ProtocolError("GA %d 长度 %d != %d" % (msg_no, len(pkt), size))
    return pkt


def _cstr(s, n):
    """定长 C 字符串：截断到 n-1 字节并补 0（保证 NUL 结尾，拍卖服会 strncpy/strlen）。"""
    if isinstance(s, str):
        s = s.encode("utf-8")
    s = s[: n - 1]
    s = s.decode("utf-8", "ignore").encode("utf-8") if s else s   # 不截断半个汉字
    return s + b"\0" * (n - len(s))


# ---------------- 结构体 ----------------
DNF_ITEM_INFO_SIZE = 53
ROI_SIZE = 12
EMBLEM_SIZE = 30
EXPANSION_SIZE = 4
GUID_SIZE = 10


def pack_item_info(item_id, add_info=0, seal=0, upgrade=0, endurance=0, extend_info=0):
    """DnfItemInfo（53 字节，pack(1)）：
    seal u8@0, item_id u32@1, uniItemAttr u8@5(低5位强化), add_info i32@6(堆叠品=数量),
    endurance u16@10, extendInfo u32@12, abilityType u8@16, abilityValue u16@17,
    guid[10]@19, RandomOption[14]@29, separate_info u8@43, reserved[9]@44
    """
    b = struct.pack("<BIBiHIBH", seal & 1, item_id, upgrade & 0x1f, add_info, endurance, extend_info, 0, 0)
    b += b"\0" * GUID_SIZE + b"\0" * 14 + b"\0" + b"\0" * 9
    assert len(b) == DNF_ITEM_INFO_SIZE
    return b


def unpack_item_info(buf, off=0):
    seal, item_id, attr, add_info, endurance, extend_info, ab_t, ab_v = struct.unpack_from("<BIBiHIBH", buf, off)
    return {
        "seal": seal, "item_id": item_id, "upgrade": attr & 0x1f, "seal_cnt": attr >> 5,
        "add_info": add_info, "endurance": endurance, "extend_info": extend_info,
        "ability_type": ab_t, "ability_value": ab_v,
        "separate_upgrade": buf[off + 43] & 0x1f,
    }


# ---------------- GA 请求 ----------------
def build_regist(gc_no=0):
    """PCK_AUCTION_REGIST_GA（0x16）：头 + gc_no（拍卖服不读，仅为对齐原版包长）。"""
    return _finish(GA_REGIST, struct.pack("<i", gc_no))


def build_check_ready():
    return _finish(GA_CHECK_AUCTION_READY, b"")


def build_ask_registed_item_num(char_idx, character_id, item_id, add_info=1, price=-1, instant_price=0, unit_price=0):
    """ASK_REGISTED_ITEM_NUM_GA（0x3d）。拍卖服只读：返回挂单数，物品不在 iteminfo.dat 时返回 -2。
    无副作用，用来做「物品是否在拍卖服 iteminfo.dat 里」的安全探测。"""
    body = struct.pack("<iiBHIiiii", char_idx, character_id, 0, 0, item_id, add_info, price, instant_price, unit_price)
    body += b"\0" * ROI_SIZE
    return _finish(GA_ASK_REGISTED_ITEM_NUM, body)


def build_regist_item(char_idx, owner_id, owner_name, item_id, count, price, instant_price, unit_price,
                      user_type=0, endurance=0):
    """PCK_AUCTION_REGIST_ITEM_GA（0xc5）。
    char_idx@18 owner_id@22 owner_name[13]@26 user_type@39 price@40 instant_price@44
    item_info[53]@48 jewel_info[30]@101 expansion[4]@131 black_point@135 guid[10]@139
    unit_price@149 roi[12]@153 owner_nexon_id[32]@165
    """
    body = struct.pack("<ii", char_idx, owner_id)
    body += _cstr(owner_name, 13)
    body += struct.pack("<Bii", user_type, price, instant_price)
    body += pack_item_info(item_id, add_info=count, endurance=endurance)
    body += b"\0" * EMBLEM_SIZE + b"\0" * EXPANSION_SIZE
    body += struct.pack("<I", 0)              # black_point
    body += b"\0" * GUID_SIZE                 # guid
    body += struct.pack("<i", unit_price)
    body += b"\0" * ROI_SIZE                  # ROI 为空（非随机属性装备）
    body += b"\0" * 32                        # owner_nexon_id（必须 NUL 结尾，拍卖服用 strlen）
    return _finish(GA_REGIST_ITEM, body)


def build_regist_cancel(char_idx, owner_id, auction_id):
    return _finish(GA_REGIST_CANCEL, struct.pack("<iiQ", char_idx, owner_id, auction_id))


def build_bidding(char_idx, buyer_id, buyer_name, price, auction_id):
    """BIDDING_GA（0x33）：char_idx@18 buyer_id@22 buyer_name[13]@26 price@39 auction_id@43。
    price == instant_price 即一口价购买。"""
    body = struct.pack("<ii", char_idx, buyer_id) + _cstr(buyer_name, 13) + struct.pack("<iQ", price, auction_id)
    return _finish(GA_BIDDING, body)


def build_buy_item_apiece(char_idx, buyer_id, price, auction_id, count):
    """BUY_ITEM_APIECE_GA（0x2a）：char_idx@18 buyer_id@22 price@26 auction_id@30 count@38。
    拍卖服要求 price == unit_price*count、挂单 price(竞价)==-1 且无出价、堆叠品。"""
    return _finish(GA_BUY_ITEM_APIECE, struct.pack("<iiiQi", char_idx, buyer_id, price, auction_id, count))


def build_search_by_itemkey(char_idx, character_id, item_ids, start_index=0,
                            upgrade_start=0, upgrade_end=31, refine_start=0, refine_end=0x7f):
    """SEARCH_BY_ITEMKEY_GA（0x81）：char_idx@18 character_id@22
    TSearchByItemId@26{startIndex u32, upStart u8, upEnd u8, itemIdNum u8, category u16, ROI[12], refStart u8, refEnd u8}
    item_id_array[20] u32 @49。
    校验（Search::FindByItem）：itemIdNum<=20，强化 0..31 且 start<=end，分离强化 0..127 且 start<=end。
    不合法时拍卖服不回包（handler 直接 return 错误码），所以这里先在本地校验。"""
    ids = list(item_ids)
    if not 1 <= len(ids) <= MAX_ITEM_ID_PER_SEARCH:
        raise ProtocolError("一次最多检索 20 个道具 ID")
    if not (0 <= upgrade_start <= upgrade_end <= 31):
        raise ProtocolError("强化范围非法")
    if not (0 <= refine_start <= refine_end <= 0x7f):
        raise ProtocolError("分离强化范围非法")
    body = struct.pack("<ii", char_idx, character_id)
    body += struct.pack("<IBBBH", start_index, upgrade_start, upgrade_end, len(ids), 0)
    body += b"\0" * ROI_SIZE
    body += struct.pack("<BB", refine_start, refine_end)
    arr = ids + [0] * (MAX_ITEM_ID_PER_SEARCH - len(ids))
    body += struct.pack("<20I", *arr)
    return _finish(GA_SEARCH_BY_ITEMKEY, body)


def build_my_registed_item_info(char_idx, owner_id):
    return _finish(GA_MY_REGISTED_ITEM_INFO, struct.pack("<ii", char_idx, owner_id))


def build_heartbeat():
    """与拍卖服心跳同形（cat=-1,id=0,len=0x12）；拍卖服 dispatch 对它直接返回，无副作用。"""
    return header(CAT_HEARTBEAT, 0, HEADER_SIZE)


# ---------------- AG 回包解析 ----------------
AUCTION_ITEM_INFO_SIZE = 0x89
MY_REGISTED_ITEM_INFO_SIZE = 0x75


def decode_name(raw):
    raw = raw.split(b"\0", 1)[0]
    for enc in ("utf-8", "gbk", "cp949"):
        try:
            return raw.decode(enc)
        except UnicodeDecodeError:
            pass
    return raw.decode("latin-1")


def parse_auction_item_info(buf, off):
    auction_id, price, instant_price, average_price = struct.unpack_from("<Qiii", buf, off)
    owner_name = decode_name(buf[off + 20: off + 33])
    (reliability,) = struct.unpack_from("<d", buf, off + 33)
    expire = buf[off + 41]
    item = unpack_item_info(buf, off + 42)
    black_point, unit_price = struct.unpack_from("<Ii", buf, off + 129)
    return {
        "auction_id": auction_id, "price": price, "instant_price": instant_price,
        "average_price": average_price, "owner_name": owner_name, "expire_hours": expire,
        "item": item, "item_id": item["item_id"], "count": item["add_info"],
        "black_point": black_point, "unit_price": unit_price,
    }


def parse_my_registed_item_info(buf, off):
    auction_id, price, instant_price = struct.unpack_from("<Qii", buf, off)
    buyer_name = decode_name(buf[off + 16: off + 29])
    expire = buf[off + 29]
    item = unpack_item_info(buf, off + 30)
    return {"auction_id": auction_id, "price": price, "instant_price": instant_price,
            "buyer_name": buyer_name, "expire_hours": expire, "item": item,
            "item_id": item["item_id"], "count": item["add_info"]}


def parse_ag(pkt):
    """把一个完整 AG 包解析成 dict。未知包返回 {'type': 'UNKNOWN', ...}。"""
    h = parse_header(pkt)
    cat, mid, size = h["cat"], h["msg_no"], h["size"]
    out = {"cat": cat, "msg_no": mid, "size": size}
    if cat == CAT_HEARTBEAT and mid == 0:
        out["type"] = "HEARTBEAT"
        return out
    if cat != CAT_AG:
        out["type"] = "OTHER_CATEGORY"
        return out
    out["type"] = AG_NAMES.get(mid, "UNKNOWN")
    b = pkt
    if mid == AG_REGIST_ACK:
        out["result"] = h["ack"]         # 0=成功 1=key 冲突(已改用备用 key) 2=服务未就绪
    elif mid == AG_CHECK_AUCTION_READY:
        out["ready"] = bool(b[18]) if size >= 19 else False
    elif mid == AG_ASK_AVERAGE_PRICE:
        v = struct.unpack_from("<iii3i3i", b, 18)
        out.update(char_idx=v[0], character_id=v[1], average_price=v[2], min_price=list(v[3:6]), min_count=list(v[6:9]))
    elif mid == AG_ASK_REGISTED_ITEM_NUM:
        char_idx, character_id, num = struct.unpack_from("<iih", b, 18)
        (item_id,) = struct.unpack_from("<I", b, 31)
        out.update(char_idx=char_idx, character_id=character_id, now_registed_num=num, item_id=item_id)
    elif mid in (AG_REGIST_RESULT, AG_REGIST_CANCEL_RESULT):
        char_idx, owner_id, result, because = struct.unpack_from("<iiBB", b, 18)
        out.update(char_idx=char_idx, owner_id=owner_id, ok=(result == 1), because=because,
                   because_text=REASON_TEXT.get(because, "") if result != 1 else "")
    elif mid in (AG_BIDDING_RESULT, AG_BUY_ITEM_APIECE):
        char_idx, buyer_id, price, result, because, auction_id = struct.unpack_from("<iiiBBQ", b, 18)
        out.update(char_idx=char_idx, buyer_id=buyer_id, price=price, ok=(result == 1), because=because,
                   because_text=REASON_TEXT.get(because, "") if result != 1 else "", auction_id=auction_id)
    elif mid == AG_LOG_MESSAGE:
        char_idx, log_type, auction_id, character_id, price, item_id, attr, add_info, endurance, ext = \
            struct.unpack_from("<iBQiiIBiHI", b, 18)
        because = b[54] if size > 54 else 0
        out.update(char_idx=char_idx, log_type=log_type, auction_id=auction_id, character_id=character_id,
                   price=price, item_id=item_id, add_info=add_info, because=because)
    elif mid == AG_ITEM_LIST:
        char_idx, character_id, pay_type, total, num = struct.unpack_from("<iiBIH", b, 18)
        if 33 + num * AUCTION_ITEM_INFO_SIZE > len(b) or num > SEARCH_PAGE:
            raise ProtocolError("ITEM_LIST 长度不符: num=%d size=%d" % (num, len(b)))
        items = [parse_auction_item_info(b, 33 + i * AUCTION_ITEM_INFO_SIZE) for i in range(num)]
        out.update(char_idx=char_idx, character_id=character_id, pay_type=pay_type, total=total, items=items)
    elif mid == AG_MY_REGISTED_ITEM_INFO:
        char_idx, owner_id, pay_type, num = struct.unpack_from("<iiBB", b, 18)
        if 28 + num * MY_REGISTED_ITEM_INFO_SIZE > len(b):
            raise ProtocolError("MY_REGISTED_ITEM_INFO 长度不符")
        items = [parse_my_registed_item_info(b, 28 + i * MY_REGISTED_ITEM_INFO_SIZE) for i in range(num)]
        out.update(char_idx=char_idx, owner_id=owner_id, items=items)
    return out


class FrameReader:
    """按 nsl 包头 sLength 切包（与 RecvBuffer::Parse 同语义）。"""

    def __init__(self, max_size=MAX_PACKET + 0x100):
        self.buf = bytearray()
        self.max_size = max_size

    def feed(self, data):
        self.buf += data
        out = []
        while len(self.buf) >= HEADER_SIZE:
            size = struct.unpack_from("<I", self.buf, 2)[0]
            if size < HEADER_SIZE or size > self.max_size:
                raise ProtocolError("非法包长 %d" % size)
            if len(self.buf) < size:
                break
            out.append(bytes(self.buf[:size]))
            del self.buf[:size]
        return out
