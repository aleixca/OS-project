# Citadel System — Project Context for Claude

## What This Project Is

A **distributed C network application** for a university OS course (La Salle, 2n IA). Five realm processes ("Maesters") run on two Linux machines and communicate over TCP using a custom **fixed 320-byte binary frame protocol** (as defined in Annex II of the assignment). Realms can forge alliances (PLEDGE), trade goods, and list each other's products.

**Run command:** `./maester <config.txt> <inventory.bin>`

**Build:** `make` (uses GCC on Linux/WSL). No build tools are available in the Windows shell; builds must be done on the Linux machines or inside WSL.

---

## Machine / Network Setup

| Machine IP | Realm | Port | Config file |
|---|---|---|---|
| 192.168.1.3 | Arryn | 8820 | `realms/arryn/arryn.txt` |
| 192.168.1.4 | Baratheon | 8821 | `realms/baratheon/baratheon.txt` |
| 192.168.1.4 | Cole | 8822 | `realms/cole/cole.txt` |
| 192.168.1.4 | Dustin | 8823 | `realms/dustin/dustin.txt` |
| 192.168.1.4 | Greyjoy | 8824 | `realms/greyjoy/greyjoy.txt` |

**Routing topology:**
- Arryn ↔ Baratheon (direct, different machines)
- Baratheon ↔ Cole, Dustin (same machine)
- Dustin ↔ Greyjoy (same machine)
- Multi-hop: Greyjoy→Arryn routes via Greyjoy→Dustin→Baratheon→Arryn
- All configs have a `DEFAULT` route as fallback

---

## Project File Structure

```
G6_P1_Code/
├── main.c                        ← entry point
├── Makefile
├── terminal/
│   ├── terminal.c                ← select() main loop (stdin + server socket + envoy pipes)
│   ├── terminal.h
│   ├── commands.c                ← parses user input into CMD_* constants
│   └── commands.h
├── utils/
│   ├── io.c                      ← to_upper, readUntil, read_screen, printF macro
│   └── io.h
├── realm/
│   ├── maester.c                 ← reads .txt config files, list_realms, exit_maester
│   └── maester.h                 ← Maester struct, Route struct
├── inventory/
│   ├── inventory.c               ← load/list/free products, stock file I/O, trade item collection, remote cache
│   └── inventory.h               ← Product, RemoteInventory, TradeItem structs
├── pledge/
│   ├── pledge.c                  ← pledge table (array of Pledge), all get/set/check functions
│   └── pledge.h                  ← PLEDGE_* status constants, Pledge struct
├── network/
│   ├── protocol.c                ← Frame struct, build_frame, checksum, parse_origin
│   ├── protocol.h
│   ├── network.c                 ← TCP socket helpers, lookup_route, send/recv_frame, broadcast_shutdown
│   └── network.h
├── transfer/
│   ├── sigil.c                   ← compute_file_md5 (fork+md5sum), send/recv file in frames
│   ├── sigil.h
│   ├── relay.c                   ← relay_pledge_hop: intermediate-hop PLEDGE relay
│   └── relay.h
├── handlers/
│   ├── message_handler.c         ← handle_incoming dispatcher + shared helpers (parse_data_fields, send_ack_*, etc.)
│   ├── message_handler.h
│   ├── handler_helpers.h         ← internal helper prototypes shared across handler .c files
│   ├── pledge_handler.c          ← handle_alliance_dest, handle_alliance_resp, send_pledge, send_pledge_response
│   ├── list_handler.c            ← handle_list_request, request_list_products
│   └── order_handler.c           ← handle_order_header, send_trade_request
├── envoy/
│   ├── envoy.c                   ← fork+pipe envoy pool (child workers for outgoing missions)
│   ├── envoy.h                   ← Envoy struct, EnvoyMission enum, all lifecycle/dispatch declarations
│   └── pipe_msg.h                ← PipeMsgHeader, PipeMsgType, write_all/read_all inline helpers
└── realms/
    ├── arryn/arryn.txt + stock.bin
    ├── baratheon/baratheon.txt + stock.bin
    ├── cole/cole.txt + stock.bin
    ├── dustin/dustin.txt + stock.bin
    └── greyjoy/greyjoy.txt + stock.bin
```

---

## Protocol (Annex II)

**Frame layout — exactly 320 bytes, packed:**
```
Offset  Size  Field
0       1     type          (MSG_* byte constant)
1       20    origin        "IP:Port" of sender's listen address
21      20    destination   realm name of target
41      2     data_length   valid bytes in data[]
43      275   data          payload (zero-padded)
318     2     checksum      sum of all other bytes % 65536
```

**Key message types:**
| Hex | Meaning |
|---|---|
| 0x01 | ALLIANCE_HEADER — PLEDGE request |
| 0x02 | SIGIL_DATA — file chunk |
| 0x03 | ALLIANCE_RESP — ACCEPT or REJECT |
| 0x11 | LIST_REQUEST |
| 0x12 | LIST_HEADER |
| 0x13 | LIST_DATA |
| 0x14 | ORDER_HEADER |
| 0x15 | ORDER_DATA |
| 0x16 | ORDER_RESP |
| 0x21 | UNKNOWN_REALM |
| 0x25 | UNAUTHORIZED |
| 0x27 | DISCONNECT |
| 0x31 | ACK_FILE (OK/KO) |
| 0x32 | ACK_MD5 (CHECK_OK/CHECK_KO) |
| 0x69 | NACK (bad checksum) |

---

## Architecture Overview

### Concurrency model (hybrid)
- **Incoming connections** → accepted in the `select()` loop → handed off to a **detached pthread** (`handle_connection_thread`). Uses threads so shared state (pledge table, products) is immediately visible.
- **Outgoing missions** (PLEDGE, LIST PRODUCTS, START TRADE, PLEDGE RESPOND) → **forked child process** with a result pipe back to parent. Results read in `select()` loop via `apply_envoy_result()`.
- `pthread_atfork` reinitialises `g_data_mutex` and `g_pledge_mutex` in the child so inherited locks don't deadlock.
- `g_data_mutex` (in envoy.c) protects `products[]` and the remote inventory cache.
- `g_pledge_mutex` (in pledge.c, recursive) protects the pledge table.

### PLEDGE flow (sending)
1. User types `PLEDGE <RealmName> <path/to/sigil.jpg>`
2. `parse_command` extracts realm + sigil path (preserving original case via `strndup` from `orig` copy)
3. Free envoy found → `dispatch_pledge()` forks child
4. Child calls `send_pledge()`:
   - Checks realm not already ALLIED
   - Checks sigil is inside `maester->user_dir`
   - Computes MD5 via `fork+execvp("md5sum")`
   - Builds 0x01 frame: `"OurRealm&SigilBasename&Size&MD5"`
   - Routes via `lookup_route()` (checks allies first, then routing table, then DEFAULT)
   - Sends 0x01 → waits 0x31 ACK → sends 0x02 chunks → waits 0x32 ACK MD5
5. Child writes `PIPE_PLEDGE_OK` or `PIPE_PLEDGE_FAIL` to result pipe, `_exit(0)`
6. Parent's `apply_envoy_result()` reads result → calls `add_outgoing_pledge()` if OK → keeps envoy ON MISSION until 0x03 response or timeout

### PLEDGE flow (receiving)
1. Incoming 0x01 frame accepted by `select()` loop → connection thread calls `handle_incoming()`
2. If `frame->destination == our realm_name` → `handle_alliance_dest()` saves sigil, verifies MD5, calls `add_incoming_pledge()`
3. Otherwise → `relay_pledge_hop()` forwards to next hop
4. User sees `>>> Alliance request received from X.`
5. User types `PLEDGE RESPOND X ACCEPT` → `dispatch_pledge_respond()` forks child → child calls `send_pledge_response()` → sends 0x03 directly to origin's listen address

### PLEDGE flow (receiving response)
1. Incoming 0x03 frame → `handle_alliance_resp()` in a connection thread
2. Checks pledge is still OUTGOING_PENDING (not timed out)
3. On ACCEPT: calls `update_pledge_status(realm, PLEDGE_ALLIED)` + `update_pledge_ip_port()`
4. Sends 0x31 ACK back to responder

---

## User Commands

| Command | Action |
|---|---|
| `LIST REALMS` | Show routing table entries |
| `LIST PRODUCTS` | Show own products |
| `LIST PRODUCTS <Realm>` | Fetch and show ally's product list |
| `PLEDGE <Realm> <sigil_path>` | Send alliance request |
| `PLEDGE STATUS` | Show all pledge statuses |
| `PLEDGE RESPOND <Realm> ACCEPT` | Accept incoming pledge |
| `PLEDGE RESPOND <Realm> REJECT` | Reject incoming pledge |
| `START TRADE <Realm>` | Begin interactive trade with ally |
| `ENVOY STATUS` | Show envoy pool status |
| `EXIT` | Graceful shutdown + DISCONNECT broadcast |
| CTRL+C | SIGINT shutdown (same as EXIT) |

---

## Config File Format

```
RealmName
realms/realmname          ← user_dir (sigils saved here)
2                         ← envoy_count
192.168.1.3               ← listen_ip
8820                      ← listen_port
--- ROUTES ---
TargetRealm IP Port       ← one route per line
DEFAULT IP Port           ← fallback route
```

---

## Key Bugs Fixed in This Session

1. **`strndup` bug in commands.c** — `orig_w2`/`orig_w3` were pointing into the uppercased `command` buffer via wrong offsets, copying too much. Fixed by computing `(size_t)(wN - command)` offset into `psOrig` buffer.

2. **`send_pledge` removed `realm_exists()` guard** — Pledges to non-directly-routed realms (e.g. Greyjoy→Arryn) were rejected with "realm doesn't exist". Now only rejects if `lookup_route()` itself fails (no route at all).

3. **Sigil ownership check** — Added: sigil path must start with `maester->user_dir` prefix. Prevents pledging with another house's sigil.

4. **Re-pledge prevention** — Added: if `get_pledge_status(realm) == PLEDGE_ALLIED`, pledge is rejected immediately.

5. **Dustin routing gaps** — Dustin had no route for Arryn/Cole; added them pointing via Baratheon.

6. **Pledge envoy lifetime** — `apply_envoy_result` keeps slot ON MISSION after `PIPE_PLEDGE_OK`; `check_pledge_envoys()` releases it when pledge status changes from `OUTGOING_PENDING`.

7. **Late ACCEPT handling** — `handle_alliance_resp` sends NACK if pledge already timed out or was never pending.

8. **CTRL+C shutdown message** — `g_nStopFlag` triggers `exit_maester()` message after terminal loop exits.

9. **`message_handler.c` split** — original ~1440-line file split into 4: `message_handler.c`, `pledge_handler.c`, `list_handler.c`, `order_handler.c`.

10. **Directory reorganization** — `sigil.c`/`relay.c` moved to `transfer/`; handlers moved to `handlers/`; Makefile updated with `-iquote transfer -iquote handlers`.

---

## La Salle Style Guide — Applied (Phases 1–6)

All 30 source files comply with the La Salle Engineering style guide:

1. **File headers** — `@File`, `@Purpose`, `@Author: Group 6`, `@Date: 2026-05-15`
2. **Include guards** — `#ifndef __FILE_H__` / `#define __FILE_H__` format
3. **No `goto`** — removed from `apply_envoy_result`; replaced with `read_ok` flag
4. **No ternary operators** — all `?:` replaced with `if/else` blocks
5. **Constant-first comparisons** — `0 == x`, `NULL == ptr` throughout
6. **Hungarian notation** — applied to all local variables, parameters, and globals:
   - `n` = int, `c` = char, `ps` = char*, `pps` = char**, `p` = pointer, `pst` = struct pointer, `st` = struct instance, `f` = float, `g_` = global prefix, `s` = char array

**Hungarian exceptions (per §4.1):** Loop counters `i`, `j`, `k`, `ei` are left as-is.

---

## Known Outstanding Issue

**Cross-machine PLEDGE fails** — When Arryn (192.168.1.3) tries to pledge Baratheon (192.168.1.4) or vice versa, the pledge doesn't work. The root cause has not yet been diagnosed. Likely candidates:
- The `create_server_socket` binds to the specific IP in the config; if that IP is wrong or the socket binding fails silently, connections from the other machine would be refused.
- The `format_origin` puts `listen_ip:listen_port` in the ORIGIN field; if the IP in the config doesn't match the actual interface IP seen by the remote machine, responses go to the wrong address.
- `md5sum` availability on the deployment machines (the sigil MD5 uses `fork+execvp("md5sum")`).
- File path issues: the sigil path check requires the sigil to be inside `maester->user_dir` — the path must be relative to the CWD where `./maester` is launched.
- The `recv_validated` 30-second timeout on sockets may be too short for cross-machine file transfers if the network is slow.

**This is the next thing to investigate and fix.**

---

## Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g \
         -iquote terminal -iquote utils -iquote realm \
         -iquote inventory -iquote pledge -iquote network \
         -iquote transfer -iquote handlers -iquote envoy

OBJ = main.o terminal/terminal.o terminal/commands.o utils/io.o \
      realm/maester.o inventory/inventory.o pledge/pledge.o \
      network/protocol.o network/network.o \
      transfer/sigil.o transfer/relay.o \
      handlers/message_handler.o handlers/pledge_handler.o \
      handlers/list_handler.o handlers/order_handler.o \
      envoy/envoy.o
```

---

## How to Test

Start each maester from the project root directory:
```bash
# On machine 192.168.1.3:
./maester realms/arryn/arryn.txt realms/arryn/stock.bin

# On machine 192.168.1.4:
./maester realms/baratheon/baratheon.txt realms/baratheon/stock.bin
./maester realms/cole/cole.txt realms/cole/stock.bin
./maester realms/dustin/dustin.txt realms/dustin/stock.bin
./maester realms/greyjoy/greyjoy.txt realms/greyjoy/stock.bin
```

**Important:** The sigil path in `PLEDGE <Realm> <path>` must be relative to CWD and inside the realm's `user_dir`. Example:
```
PLEDGE Baratheon realms/arryn/sigil.jpg
```
