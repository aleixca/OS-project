# Citadel System — Global Test Guide

## Setup

```bash
bash setup.sh      # build + verify files
bash launch.sh     # open 5 terminals
```

**Topology**
```
A:Arryn(5001,env=2) ─── B:Baratheon(5002,env=3) ─── C:Cole(5003,env=1)
                                  │
                         D:Dustin(5004,env=1) ─── E:Greyjoy(5005,env=2)
```

Sigils available: `sigils/arryn.png  sigils/baratheon.png  sigils/cole.png`
                  `sigils/dustin.png  sigils/greyjoy.png`

---

## PHASE 1 — Basic Terminal

### T1.1 — LIST REALMS
**In any Maester terminal:**
```
$ LIST REALMS
```
Expected: lists all known realms from the routing table.

### T1.2 — LIST PRODUCTS (own)
**In any Maester terminal:**
```
$ LIST PRODUCTS
```
Expected: shows own inventory with product names, quantities, weights.

### T1.3 — PLEDGE STATUS (empty)
**In any Maester terminal:**
```
$ PLEDGE STATUS
```
Expected: `You have no pledges awaiting or accepted`

### T1.4 — ENVOY STATUS (all free)
**In any Maester terminal:**
```
$ ENVOY STATUS
```
Expected (Arryn): `- Envoy 1: FREE` / `- Envoy 2: FREE`
Expected (Baratheon): three FREE envoys.

### T1.5 — Unknown command
```
$ FOOBAR
```
Expected: `Unknown command.`

---

## PHASE 2 — Routing

### T2.1 — PLEDGE invalid realm
**In Arryn:**
```
$ PLEDGE NonExistentRealm sigils/arryn.png
```
Expected: `No such realm exists. The pledge is hereby withdrawn.`

### T2.2 — PLEDGE missing sigil
**In Arryn:**
```
$ PLEDGE Baratheon sigils/nonexistent.png
```
Expected: `Sigil not found. The pledge is hereby withdrawn.`

---

## PHASE 3 — File Transfer: PLEDGE

### T3.1 — Direct PLEDGE (A → B, 1 hop away)
**In Arryn (A):**
```
$ PLEDGE Baratheon sigils/arryn.png
```
Expected: `>>> Pledge dispatched to Baratheon. Awaiting their response.`
Arryn's Envoy 1 goes ON MISSION.

**In Baratheon (B)** — receives automatically:
```
>>> Alliance request received from Arryn.
```

**In Baratheon (B):**
```
$ PLEDGE RESPOND Arryn ACCEPT
```
Expected: `Alliance with Arryn established.`

**In Arryn (A)** — receives automatically:
```
>>> Alliance with Baratheon forged successfully!
```

### T3.2 — PLEDGE through 1 hop (C → A, goes through B)
**In Cole (C):**
```
$ PLEDGE Arryn sigils/cole.png
```
**In Baratheon (B)** — hop log appears automatically:
```
>>> Received hop: Cole -> Arryn (PLEDGE)
Found route: Arryn -> 127.0.0.1:5001
Forwarding...
```
**In Arryn (A)** — receives sigil:
```
>>> Alliance request received from Cole.
```
**In Arryn (A):**
```
$ PLEDGE RESPOND Cole ACCEPT
```

### T3.3 — PLEDGE through 2 hops (A → D, goes A→B→D)
**In Arryn (A):**
```
$ PLEDGE Dustin sigils/arryn.png
```
Baratheon sees hop log. Dustin receives sigil.

**In Dustin (D):**
```
$ PLEDGE RESPOND Arryn ACCEPT
```

### T3.4 — PLEDGE through 3 hops (A → E, goes A→B→D→E)
**In Arryn (A):**
```
$ PLEDGE Greyjoy sigils/arryn.png
```
Baratheon and Dustin both show hop logs.
Greyjoy receives sigil.

**In Greyjoy (E):**
```
$ PLEDGE RESPOND Arryn ACCEPT
```

### T3.5 — PLEDGE timeout (do not respond for 120 seconds)
**In Cole (C):** (if not already allied with Baratheon)
```
$ PLEDGE Baratheon sigils/cole.png
```
Wait 120 seconds without responding in Baratheon.
**In Cole (C)** — fires automatically:
```
>>> Pledge to Baratheon has failed (TIMEOUT).
```

### T3.6 — PLEDGE RESPOND after timeout
After T3.5 timeout, if Baratheon then types:
```
$ PLEDGE RESPOND Cole ACCEPT
```
Expected: `Alliance with Cole established.` in Baratheon, but Cole gets a KO back — **alliance is NOT formed** (timeout already rejected it).

---

## PHASE 3 — LIST PRODUCTS

### T3.7 — LIST PRODUCTS when not allied
**In Cole (C):** (before allying with Arryn)
```
$ LIST PRODUCTS Arryn
```
Expected: `The gates of commerce with Arryn remain closed; no alliance binds you.`

### T3.8 — LIST PRODUCTS from allied realm
First establish A↔B alliance (T3.1), then in Arryn:
```
$ LIST PRODUCTS Baratheon
```
Expected: numbered list of Baratheon's products with units.
Baratheon terminal shows: `>>> LIST PRODUCTS request from Arryn. Products delivered.`

### T3.9 — START TRADE without LIST PRODUCTS first
**In Arryn (A)** (after allying with Baratheon but before listing products):
```
$ START TRADE Baratheon
```
Expected: `No products available. Use LIST PRODUCTS first.`

---

## PHASE 3 — START TRADE

### T3.10 — Successful trade
After T3.8 (LIST PRODUCTS Baratheon done):
**In Arryn (A):**
```
$ START TRADE Baratheon
```
Expected header: `Trade with Baratheon begins.` + available products list.
```
(trade)> ADD <ProductName> 5
(trade)> SEND
```
Expected in Arryn: `>>> Order accepted by Baratheon. Stock updated.`
Expected in Baratheon: `>>> Trade request received from Arryn. Order processed successfully. Stock updated.`

Verify: LIST PRODUCTS in both terminals shows updated quantities.

### T3.11 — Trade rejected: OUT_OF_STOCK
**In Arryn (A):**
```
$ START TRADE Baratheon
(trade)> ADD <ProductName> 999999
(trade)> SEND
```
Expected in Arryn: `>>> Order rejected by Baratheon: OUT_OF_STOCK`

### T3.12 — Trade rejected: UNKNOWN_PRODUCT
**In Arryn (A):**
```
$ START TRADE Baratheon
(trade)> ADD DRAGON_EGGS 1
```
Expected: `Product not available from this realm.`
(Local validation from cached product list prevents sending.)

### T3.13 — Trade CANCEL
```
$ START TRADE Baratheon
(trade)> ADD <ProductName> 1
(trade)> CANCEL
```
Expected: `Trade cancelled.` — envoy released, no network activity.

---

## PHASE 4 — Envoys

### T4.1 — ENVOY STATUS at startup
**In Baratheon (B):**
```
$ ENVOY STATUS
- Envoy 1: FREE
- Envoy 2: FREE
- Envoy 3: FREE
```

### T4.2 — Concurrent PLEDGEs
**In Baratheon (B):**
```
$ PLEDGE Cole sigils/baratheon.png
$ ENVOY STATUS
```
Expected: Envoy 1 ON MISSION (PLEDGE to Cole), Envoy 2 FREE, Envoy 3 FREE.
```
$ PLEDGE Dustin sigils/baratheon.png
$ ENVOY STATUS
```
Expected: Envoy 1 ON MISSION, Envoy 2 ON MISSION, Envoy 3 FREE.
```
$ PLEDGE Greyjoy sigils/baratheon.png
$ ENVOY STATUS
```
Expected: all 3 ON MISSION.

### T4.3 — All envoys occupied
**In Baratheon (B)** with all 3 envoys busy:
```
$ PLEDGE Arryn sigils/baratheon.png
```
Expected: `All envoys are occupied. Your command must wait.`
```
$ LIST PRODUCTS Cole
```
Expected: `All envoys are occupied. Your command must wait.`
```
$ START TRADE Cole
```
Expected: `All envoys are occupied. Your command must wait.`

### T4.4 — Envoy freed after response
Wait for one of the pledges in T4.2 to receive a RESPOND.
After the response, one envoy becomes FREE:
```
$ ENVOY STATUS
- Envoy 1: FREE
- Envoy 2: ON MISSION (PLEDGE to Dustin)
- Envoy 3: ON MISSION (PLEDGE to Greyjoy)
```
Now `PLEDGE Arryn sigils/baratheon.png` should work again.

### T4.5 — LIST PRODUCTS with free envoy
After allying B↔C (T4.2), in Baratheon:
```
$ LIST PRODUCTS Cole
```
Expected: dispatched to free envoy, prompt returns immediately, product list printed when received.

### T4.6 — START TRADE envoy reservation
**In Baratheon (B)** with 2 envoys busy, 1 free:
```
$ START TRADE Cole
```
Expected: enters `(trade)>` menu immediately (envoy reserved).
While in the trade menu, in another window check envoy status — the reserved envoy shows ON MISSION.
Type `CANCEL` to release the envoy.

---

## PHASE 3/4 — Disconnect

### T5.1 — Clean EXIT
**In Greyjoy (E)** (after allying with Dustin):
```
$ EXIT
```
Expected in Dustin: `[DISCONNECT] Realm Greyjoy has disconnected.`
Greyjoy process exits cleanly.

### T5.2 — CTRL+C shutdown
Press `CTRL+C` in any allied Maester.
Expected: allied Maesters receive the disconnect frame automatically.

---

## Full Integration Test (run in order)

1. Start all 5 Maesters: `bash launch.sh`
2. **T1.3, T1.4** — Verify PLEDGE STATUS empty, ENVOY STATUS all FREE
3. **T3.1** — Arryn ↔ Baratheon alliance (direct, 1 hop)
4. **T3.2** — Cole → Arryn (through Baratheon, hop relay visible)
5. **T3.3** — Arryn → Dustin (2 hops)
6. **T3.4** — Arryn → Greyjoy (3 hops, longest path)
7. **T4.2** — Baratheon sends 3 simultaneous PLEDGEs
8. **T4.3** — 4th PLEDGE/LIST/TRADE blocked with "All envoys occupied"
9. **T3.8** — LIST PRODUCTS Baratheon from Arryn
10. **T3.10** — Successful trade Arryn buys from Baratheon
11. **T3.11** — Rejected trade (OUT_OF_STOCK)
12. **T5.1** — Clean EXIT from Greyjoy
