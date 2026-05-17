# Citadel System — Validation Test Suite

## Topology & Setup

```
A:Arryn(5001, 2 envoys) ─── B:Baratheon(5002, 3 envoys) ─── C:Cole(5003, 1 envoy)
                                       │
                              D:Dustin(5004, 1 envoy) ─── E:Greyjoy(5005, 2 envoys)
```

```bash
make clean && make          # build
./maester realms/arryn/arryn.txt       realms/arryn/inventory.bin       # terminal A
./maester realms/baratheon/baratheon.txt realms/baratheon/inventory.bin # terminal B
./maester realms/cole/cole.txt         realms/cole/inventory.bin        # terminal C
./maester realms/dustin/dustin.txt     realms/dustin/inventory.bin      # terminal D
./maester realms/greyjoy/greyjoy.txt   realms/greyjoy/inventory.bin     # terminal E
```

Sigils: `realms/arryn/sigils/` (or wherever they are configured)

---

## Section 1 — Basic Terminal Commands

### T1.1 LIST REALMS
In any terminal:
```
$ LIST REALMS
```
**Expected:** All realms from the routing table are printed, one per line. No crash.

### T1.2 LIST PRODUCTS (own)
In any terminal:
```
$ LIST PRODUCTS
```
**Expected:** Trade Ledger table with product names, amounts, weights. Total Entries matches the inventory file.

### T1.3 PLEDGE STATUS (empty)
In any terminal before any pledge:
```
$ PLEDGE STATUS
```
**Expected:** `You have no pledges awaiting or accepted`

### T1.4 ENVOY STATUS (startup)
In Arryn (2 envoys):
```
$ ENVOY STATUS
```
**Expected:** `- Envoy 1: FREE` and `- Envoy 2: FREE`

In Baratheon (3 envoys): three FREE lines.

### T1.5 Unknown command
```
$ FOOBAR
$ LIST
$ PLEDGE
$ PLEDGE RESPOND
```
**Expected:** appropriate error messages for each; no crash, prompt returns.

### T1.6 Extra arguments rejected
```
$ LIST REALMS extra
$ PLEDGE STATUS extra
$ ENVOY STATUS extra
```
**Expected:** `Unknown command.` for each.

---

## Section 2 — Routing & Error Handling

### T2.1 PLEDGE to non-existent realm
In Arryn:
```
$ PLEDGE Lannister sigils/arryn.png
```
**Expected:** `No such realm exists. The pledge is hereby withdrawn.`

### T2.2 PLEDGE with missing sigil
In Arryn:
```
$ PLEDGE Baratheon sigils/notafile.png
```
**Expected:** `Sigil not found. The pledge is hereby withdrawn.`

### T2.3 Wildcard route falls through to DEFAULT
Configure a realm with `*.*.*.* 0` in its routing table (e.g., add `TestRealm *.*.*.* 0` to arryn.txt routes). Start Arryn.
```
$ PLEDGE TestRealm sigils/arryn.png
```
**Expected:** Uses DEFAULT route instead of attempting to connect to `*.*.*.*`. The pledge is either forwarded through DEFAULT or fails gracefully — no crash, no connection attempt to address `0.0.0.0`.

### T2.4 START TRADE without prior LIST PRODUCTS
Ally A↔B first (see T3.1), then in Arryn:
```
$ START TRADE Baratheon
```
**Expected:** `No products available. Use LIST PRODUCTS first.`

### T2.5 LIST PRODUCTS for unallied realm
In Arryn (before allying with Cole):
```
$ LIST PRODUCTS Cole
```
**Expected:** `The gates of commerce with Cole remain closed; no alliance binds you.`

---

## Section 3 — PLEDGE Protocol

### T3.1 Direct PLEDGE and ACCEPT (1 hop, A→B)
**Arryn:**
```
$ PLEDGE Baratheon sigils/arryn.png
```
**Expected (Arryn):** `>>> Pledge dispatched to Baratheon. Awaiting their response.`
Envoy 1 shows ON MISSION.

**Expected (Baratheon):** `>>> Alliance request received from Arryn.`

**Baratheon:**
```
$ PLEDGE RESPOND Arryn ACCEPT
```
**Expected (Baratheon):** `Alliance with Arryn established.`
**Expected (Arryn):** `>>> Alliance with Baratheon forged successfully!`

Check both sides:
```
$ PLEDGE STATUS
```
**Expected:** entry shows `ACCEPTED` for the allied realm.

### T3.2 Direct PLEDGE and REJECT
**Cole** (before any alliance with Baratheon):
```
$ PLEDGE Baratheon sigils/cole.png
```
**Baratheon:**
```
$ PLEDGE RESPOND Cole REJECT
```
**Expected (Baratheon):** `REJECT sent to Cole.`
**Expected (Cole):** `>>> Alliance with Baratheon was refused!`

Cole PLEDGE STATUS shows `REJECTED`.

### T3.3 PLEDGE through 1 hop (C→A, relayed through B)
**Cole:**
```
$ PLEDGE Arryn sigils/cole.png
```
**Expected (Baratheon):** hop log `>>> Received hop: Cole -> Arryn (PLEDGE)` and `Forwarding...`
**Expected (Arryn):** `>>> Alliance request received from Cole.`

**Arryn:**
```
$ PLEDGE RESPOND Cole ACCEPT
```
**Expected:** both sides show alliance established.

### T3.4 PLEDGE through 2 hops (A→D, path A→B→D)
**Arryn:**
```
$ PLEDGE Dustin sigils/arryn.png
```
**Expected (Baratheon):** hop log (forwards to Dustin)
**Expected (Dustin):** `>>> Alliance request received from Arryn.`

**Dustin:**
```
$ PLEDGE RESPOND Arryn ACCEPT
```

### T3.5 PLEDGE through 3 hops (A→E, path A→B→D→E)
**Arryn:**
```
$ PLEDGE Greyjoy sigils/arryn.png
```
**Expected:** Baratheon and Dustin each show a hop log. Greyjoy receives the sigil.

**Greyjoy:**
```
$ PLEDGE RESPOND Arryn ACCEPT
```

### T3.6 PLEDGE timeout (120 seconds)
**Cole** (if not already allied with Baratheon):
```
$ PLEDGE Baratheon sigils/cole.png
```
Do not respond in Baratheon. Wait 120 seconds.

**Expected (Cole):** `>>> Pledge to Baratheon has failed (TIMEOUT).`
**Expected (Cole PLEDGE STATUS):** `REJECTED`

Verify envoy in Cole is released (FREE) immediately after timeout — the condition variable wakeup should happen within 1 second of the timeout firing, not after a 200 ms poll delay.

### T3.7 Late PLEDGE RESPOND after timeout
Continuing from T3.6 — Baratheon now types:
```
$ PLEDGE RESPOND Cole ACCEPT
```
**Expected (Baratheon):** `Alliance with Cole established.`
**Expected (Cole):** receives a NACK frame (not ACK KO). Cole's pledge stays REJECTED. **Alliance is NOT formed on Cole's side.**

### T3.8 Realm name case preserved in frames
In Arryn (config has `Baratheon`, not `BARATHEON`):
```
$ PLEDGE Baratheon sigils/arryn.png
```
**Expected:** frames on the wire use `Baratheon` in DESTINATION, not `BARATHEON`. Verify in Baratheon that the alliance is recorded under the correct name `Arryn` (not `ARRYN`).

---

## Section 4 — LIST PRODUCTS Protocol

### T4.1 Successful LIST PRODUCTS
After A↔B alliance (T3.1):

**Arryn:**
```
$ LIST PRODUCTS Baratheon
```
**Expected (Arryn):** numbered product list from Baratheon's inventory.
**Expected (Baratheon):** `>>> LIST PRODUCTS request from Arryn. Products delivered.`

### T4.2 LIST PRODUCTS authorization — forged DATA rejected
This tests Issue 10 (DATA realm cross-checked against origin IP).

If a custom client sends a 0x11 frame claiming `DATA = Arryn` but from an IP that does not belong to Arryn in the pledge table, Baratheon must send `MSG_UNAUTHORIZED` and not deliver the product list.

**Manual/scripted test:** Send a raw 0x11 frame to Baratheon with `DATA = Arryn` from a non-Arryn IP:Port.
**Expected (Baratheon):** `MSG_UNAUTHORIZED` response. Product list not transmitted.

---

## Section 5 — TRADE Protocol

### T5.1 Successful trade (normal flow)
After A↔B alliance and Arryn has listed Baratheon products (T4.1):

**Arryn:**
```
$ START TRADE Baratheon
(trade)> ADD <ProductName> 2
(trade)> SEND
```
**Expected (Arryn):** `>>> Order accepted by Baratheon. Stock updated.`
**Expected (Baratheon):** `>>> Trade request processed. Order fulfilled. Stock updated.`

Verify both inventories updated:
```
$ LIST PRODUCTS
```

### T5.2 Trade CANCEL
**Arryn:**
```
$ START TRADE Baratheon
(trade)> ADD <ProductName> 1
(trade)> CANCEL
```
**Expected:** `Trade cancelled.` Stock unchanged on both sides.

### T5.3 Trade rejected — OUT_OF_STOCK
**Arryn:**
```
$ START TRADE Baratheon
(trade)> ADD <ProductName> 999999
(trade)> SEND
```
**Expected (Arryn):** `>>> Order rejected by Baratheon: OUT_OF_STOCK`
**Expected (Baratheon):** `>>> Trade rejected: OUT_OF_STOCK`
Stock unchanged on both sides.

### T5.4 Trade rejected — UNKNOWN_PRODUCT
**Arryn:**
```
$ START TRADE Baratheon
(trade)> ADD DRAGONGLASS 1
```
**Expected (Arryn):** `Product not available from this realm.` (local check, no network traffic)

### T5.5 Duplicate order lines do not oversell
This tests Issue 3. Create a trade order file manually (or intercept the trade) that contains the same product twice:
```
ProductName&8
ProductName&8
```
with Baratheon having stock 10.

**Expected (Baratheon):** `REJECT&OUT_OF_STOCK` — the aggregated total (16) exceeds stock (10). Stock is NOT reduced.

Alternatively test via the trade menu: run two separate trades of 8 each and verify the second is rejected:
- Trade 1: ADD ProductName 8 → SEND → accepted (stock goes to 2)
- Trade 2: ADD ProductName 8 → SEND → rejected OUT_OF_STOCK (stock stays 2)

### T5.6 Multi-word product names
If any inventory has a product with a multi-word name (e.g., `Myrish Lace`):
**Arryn:**
```
$ START TRADE Baratheon
(trade)> ADD Myrish Lace 3
(trade)> SEND
```
**Expected:** product is found, added to order, and the trade succeeds normally. No "Product not available" error.

Test with up to 5 words if such products exist in the inventory.

### T5.7 Invalid quantity rejected
```
(trade)> ADD <ProductName> 0
(trade)> ADD <ProductName> -1
(trade)> ADD <ProductName> abc
(trade)> ADD <ProductName> 10abc
```
**Expected:** each is rejected with `Invalid amount. Must be a positive integer.` or `Invalid command.`

---

## Section 6 — Envoy Concurrency

### T6.1 ENVOY STATUS tracks missions
In Baratheon (3 envoys):
```
$ PLEDGE Cole sigils/baratheon.png
$ ENVOY STATUS
```
**Expected:** Envoy 1 shows `ON MISSION (PLEDGE to Cole)`, 2 and 3 FREE.

```
$ PLEDGE Dustin sigils/baratheon.png
$ PLEDGE Greyjoy sigils/baratheon.png
$ ENVOY STATUS
```
**Expected:** all 3 ON MISSION.

### T6.2 All envoys occupied — commands blocked
With all 3 Baratheon envoys busy:
```
$ PLEDGE Arryn sigils/baratheon.png
```
**Expected:** `All envoys are occupied. Your command must wait.`
```
$ LIST PRODUCTS Cole
$ START TRADE Cole
```
**Expected:** same message for each. Prompt returns immediately.

### T6.3 Envoy freed on pledge response
Wait for one of the pledges in T6.1 to be responded to. After the response:
```
$ ENVOY STATUS
```
**Expected:** the corresponding envoy shows FREE. A previously blocked command now works.

### T6.4 Condition variable — no busy wait
Start Baratheon and issue a PLEDGE to Cole. While the pledge is OUTGOING_PENDING, observe CPU usage (e.g., `top`).
**Expected:** the envoy thread uses near-zero CPU while waiting for the response. It wakes immediately (< 1 second) when Cole responds, not after a polling interval.

### T6.5 Concurrent LIST PRODUCTS and START TRADE
With A↔B alliance, run simultaneously from Arryn:
- Terminal 1: `$ LIST PRODUCTS Baratheon` (re-lists)
- Terminal 2: `$ START TRADE Baratheon` (enters trade menu)

**Expected:** no crash, no stale pointer dereference, trade menu shows correct products, LIST PRODUCTS completes normally.

### T6.6 Trade envoy reservation
In Baratheon (3 envoys, 2 busy):
```
$ START TRADE Cole
```
**Expected:** enters `(trade)>` immediately. Envoy count drops to 0 free. A 4th command is blocked.
```
(trade)> CANCEL
```
**Expected:** envoy freed, ENVOY STATUS shows it as FREE again.

---

## Section 7 — Protocol Compliance

### T7.1 ACK FILE and ACK MD5 have empty ORIGIN/DESTINATION
Capture frames during a PLEDGE handshake (e.g., with `tcpdump` or a frame logger).
**Expected:** 0x31 (ACK FILE) and 0x32 (ACK MD5) frames have empty ORIGIN and DESTINATION fields. The DATA field contains `OK&RealmName` or `CHECK_OK&RealmName`.

### T7.2 NACK on bad checksum
Send a frame with a corrupted checksum to any Maester.
**Expected:** receiver sends back a 0x69 NACK frame and discards the original frame.

### T7.3 NACK for unknown frame type
Send a frame with an undefined type byte to a Maester.
**Expected:** Maester responds with 0x69 NACK and continues normally.

### T7.4 UNKNOWN_REALM destination is realm name, not IP:Port
Attempt a PLEDGE to a realm that exists in the routing table of A but is not reachable through B (e.g., route lookup fails at B).
**Expected:** the 0x21 UNKNOWN_REALM frame returned to A has DESTINATION set to the initiating realm name (`Arryn`), not B's IP:Port string.

### T7.5 Unauthorized order response destination is realm name
Send an order from an unallied realm (or forge a 0x14 frame from an unknown IP).
**Expected:** the 0x21 UNAUTHORIZED frame has DESTINATION set to the resolved realm name if known, or empty string if unknown — never an `IP:Port` string.

### T7.6 Config & sanitisation
Create a test config file with `&` characters in the realm name or route names. Start the Maester.
**Expected:** all `&` characters are stripped at startup. The realm name and route names displayed by LIST REALMS contain no `&`.

---

## Section 8 — Shutdown & Cleanup

### T8.1 Clean EXIT with allies
After establishing at least one alliance (e.g., A↔B):
**Greyjoy:**
```
$ EXIT
```
**Expected:** allied Maesters (Dustin if allied) receive `[DISCONNECT] Realm Greyjoy has left the realm.` Greyjoy process exits with code 0 and no memory errors (run under `valgrind` for full check).

### T8.2 CTRL+C shutdown
Press `CTRL+C` in any Maester with active allies.
**Expected:** DISCONNECT frames sent to all allies. Allies print disconnect message. Process exits cleanly.

### T8.3 Shutdown with in-flight connections
Start a long LIST PRODUCTS or TRADE while simultaneously issuing EXIT on the other side.
**Expected:** the EXIT waits for active connection threads and envoy threads to drain before freeing resources. No crash or double-free.

### T8.4 Socket timeout — stalled peer
Connect a raw socket to a Maester and stop sending data mid-handshake (e.g., send the 0x01 header then go silent).
**Expected:** the connection thread times out after 30 seconds and exits cleanly. The Maester continues accepting new connections normally.

---

## Section 9 — Stress / Regression

### T9.1 Full alliance mesh
Establish all possible alliances: A↔B, A↔C, A↔D, A↔E, B↔C, B↔D, B↔E, D↔E.
**Expected:** no crashes, PLEDGE STATUS on each node shows all allies as ACCEPTED, each node can LIST PRODUCTS from every ally.

### T9.2 Trade across entire mesh
After T9.1, run a trade between every pair of allied realms.
**Expected:** all trades succeed or fail with correct error messages. No stale pointer, no double-free.

### T9.3 Rapid re-pledge after failure
In Cole, pledge to Baratheon, wait for rejection, then immediately re-pledge:
```
$ PLEDGE Baratheon sigils/cole.png
$ PLEDGE RESPOND Cole REJECT   (in Baratheon)
$ PLEDGE Baratheon sigils/cole.png  (immediately in Cole)
```
**Expected:** second pledge dispatched successfully. Pledge table re-uses the existing entry correctly.

### T9.4 valgrind clean build
Run any single-realm scenario under valgrind:
```bash
valgrind --dsymutil=yes --track-origins=yes --leak-check=full \
         --track-fds=yes --show-reachable=yes -s \
         ./maester realms/arryn/arryn.txt realms/arryn/arryn_stock.db
```
Perform LIST PRODUCTS (own), PLEDGE STATUS, ENVOY STATUS, then EXIT.
**Expected:** zero memory errors, zero leaks (or only still-reachable from pthreads internals which is expected).

---

## Full Integration Sequence (run in order)

| Step | Action | Verify |
|------|--------|--------|
| 1 | Start all 5 Maesters | Each prints startup message with realm name |
| 2 | T1.3, T1.4 | PLEDGE STATUS empty, ENVOY STATUS all FREE |
| 3 | T3.1 | A↔B alliance, both show ACCEPTED |
| 4 | T4.1 | Arryn lists Baratheon products |
| 5 | T5.1 | Arryn buys 2 units from Baratheon, stock updated both sides |
| 6 | T5.3 | Arryn tries to buy 999999 units, rejected OUT_OF_STOCK |
| 7 | T3.3 | Cole → Arryn through B (hop log visible in Baratheon) |
| 8 | T3.4 | Arryn → Dustin through B→D (2 hops) |
| 9 | T3.5/T3.7 | Cole pledges Baratheon, timeout fires, late RESPOND sends NACK not KO |
| 10 | T6.1, T6.2 | Baratheon sends 3 simultaneous PLEDGEs, 4th blocked |
| 11 | T6.3 | After a response, envoy freed, next command dispatched |
| 12 | T5.5 | Duplicate order lines correctly rejected (aggregated check) |
| 13 | T5.6 | Multi-word product name trade succeeds |
| 14 | T8.1 | Greyjoy exits cleanly, allies notified |
| 15 | T8.2 | CTRL+C on remaining Maester, allies notified |
