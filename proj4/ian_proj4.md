### IAN - Projekt 4
Marek Gergel (xgerge01)

---

#### "Zákazník"/"Klient" #1:
*Náš progam "program-1" zaťažuje jedno celé CPU, ale vačšina cpu-usage je %system. Viete nám vysvetliť prečo? Je to nejaká chyba v kernelu?*

Ak program zaťažuje jedno celé CPU, ale väčšina cpu-usage je %system, znamená to, že program často volá systémové volania. Tieto volania môžu byť napr. z dôvodu čítania alebo zapisovania do súboru, alebo iné operácie, ktoré vyžadujú interakciu so systémom.

Pomocou nástroja `strace` som zistil, že program v smyčke do "nekonečna" prepisuje v súbore rovnaké dáta na rovnaké miesto.
```
lseek(3, 0, SEEK_SET)                   = 0
write(3, "This is a 64-byte string to be w"..., 51) = 51
```

Po zavolaní `strace -C` sa v ukončení programu vypíše štatistika systémových volaní, ktoré program vykonal. Najviac systémových volaní bolo práve na `write` a `lseek`, čo je príčinou vysokého vyťaženia cpu systémom. Chybou nie je kernel, ale program, ktorý neefektívne využíva systémové volania.
```
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 60.03    0.285269           1    185047           write
 39.82    0.189198           1    185048           lseek
  0.04    0.000181          22         8           mmap
  0.03    0.000151          50         3           openat
  ...
```

---

#### "Zákazník"/"Klient" #2:
*Máme HPC (High-Performance Computing) program "program-2" a zaujímalo by nás, jak moc je jeho výkon (čas do ukončenia programu) degradovaný prerušeniami a prípadne inými asynchronnými systémovými a kernelovými rutinami. Viete nám pomôcť zmerať tento "system/kernel overhead"?*

*(Pozn.: Nemusíte nutne zahrnúť úplne všetky možné prerušenia a overhead, stačí ak vytrasujte a spočítate aspoň nejaký výrazný (napr. kernel timery). Hodnotiť sa bude hlavne spôsob ako ste ku svojmu záveru dospeli.)*

Program vypisuje nějaký loop_time, např "Loop time: 12.566891 seconds", vždy okolo 12s.
Strace dává "podezřelé" pouze dva řádky:

```
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, {tv_sec=0, tv_nsec=3236405}) = 0
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, {tv_sec=12, tv_nsec=570127576}) = 0
```

Nastavuje se timer, který zjistí, jak dlouho běžel proces.

Pomocí kernelového nástroje pro trasování `ftrace` bude možné získat informace o přerušeních po dobu trvání programu.
Použijeme nástroj `trace-cmd`, což je pohodlnější frontend pro `ftrace`.

Následující příkaz nám vypíše všechna top-level volání v kernelu po dobu obsluhy přerušení.
`trace-cmd record -p function_graph --max-graph-depth 1 -e interrupt_handler ./program-2`

Ten pak pomocí `trace-cmd report` lze vizualizovat.

```
...
     program-2-55050 [001] 6799173.549379: funcgraph_entry:        5.357 us   |  __sysvec_apic_timer_interrupt();
     program-2-55050 [001] 6799173.549384: funcgraph_entry:        2.237 us   |  irq_exit_rcu();
     program-2-55050 [001] 6799173.549386: funcgraph_entry:                   |  exit_to_user_mode_prepare() {
xfsaild/dm-0-545   [001] 6799173.549388: funcgraph_exit:       # 50997.741 us |  }
  xfsaild/dm-0-545   [001] 6799173.549388: funcgraph_entry:        0.094 us   |  __cond_resched();
  xfsaild/dm-0-545   [001] 6799173.549388: funcgraph_entry:        0.095 us   |  __cond_resched();
  xfsaild/dm-0-545   [001] 6799173.549389: funcgraph_entry:        0.447 us   |  xfsaild_push();
...
```

Pomocí `trace-cmd report | grep program-2` si vyfiltrujeme pouze přerušení v rámci sledovaného programu.
Pomocí `trace-cmd report | grep program-2 | grep timer` si vyfiltrujeme pouze přerušení časovače, jak avizovalo zadání.
Pomocí `trace-cmd report | grep program-2 | grep timer | awk 'BEGIN{sum = 0;}{sum += $5;}END{print sum;}'` si sečteme celkový čas zabraný obsluhou časovačů.

Výsledek: 76602,2 us = 76 ms

Což v poměru s programem nahlášeným 12s loop-time je zanedbatelné číslo.
Performance jistě nebude degradována kernelovým/systémovým overhead.

---

#### "Zákazník"/"Klient" #3:
*Máme problém s naším viac-procesovým (multithread) programom. Keď spustíme iba jedno vlákno "program-3-single", výkon (čas na spočítanie jedného cyklu) je podstatne lepší než keď spustíme viac-procesovú formu "program-3-multi". Pre porovnanie:*
```
  ]# ./program-3-single 
  Loop time: 1.938028 seconds
  Loop time: 1.491816 seconds
  Loop time: 1.180750 seconds
  Loop time: 1.184299 seconds


  ]# ./program-3-multi
  Loop time: 1.947351 seconds
  Loop time: 1.946225 seconds
  Loop time: 1.951270 seconds
  Loop time: 1.952191 seconds
```
*Naši developeri si myslia, že je problém niekde s plánovaním procesov. Viete nám pomôcť potvrdiť túto hypotézu, prípadne zistit kde inde je problém a ako by sme ho mohli vyriešiť?*

Z nasledujúcich výstupov je zrejmé, že problém je v plánovaní procesov. Pri viac-procesovej forme programu program-3-multi je oveľa vyššia latencia pri prebudení procesu oproti jedno-vláknovej forme programu program-3-single.

`trace-cmd record -e sched_switch -e sched_wakeup ./program-3-single`  
`trace-cmd report -w`  
```
...
Average wakeup latency: 11.132 usecs
Maximum Latency: 263.878 usecs at timestamp: 6743859.891000
Minimum Latency: 0.419 usecs at timestamp: 6743861.554701
...
```

`trace-cmd record -e sched_switch -e sched_wakeup ./program-3-multi`  
`trace-cmd report -w`  
```
...
Average wakeup latency: 45202.593 usecs
Maximum Latency: 953985.869 usecs at timestamp: 6742276.412417
Minimum Latency: 1.454 usecs at timestamp: 6742292.411601
...
```

Pomocou príkazu `strace ./program-3-multi | grep sched` som zistil, že program-3-multi volá `sched_setaffinity`, kde ako druhý argument je maska CPU vlákien a určuje na ktorých vláknach môže proces bežať. Hodnota 128 (0b10000000) znamená, že proces môže byť spustený iba na jednom CPU vlákne.
```
sched_setaffinity(0, 128, [0])          = 0
sched_getparam(0, [0])                  = 0
sched_setscheduler(0, SCHED_FIFO, [50]) = 0
```

Pomocou príkazu `strace -f -T ./program-3-multi` a následného `grep`-ovania výstupu jedného z child procesu som zistil, že child proces je vytvoréný volaním `clone`, ktoré rovnako ako parent process má povolené iba jedno vlákno na CPU. Child proces si taktiež počas "merania času výpočtu" (volania `clock_gettime()`) volá `sched_setscheduler(0, SCHED_RR, [30])`, kde mení nastavenie plánovača procesov, čo môže ovplyvniť výkon programu. 

```
[pid 63411] clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLDstrace: Process 63413 attached
, child_tidptr=0x7fe49186fa10) = 63413 <0.000560>
[pid 63413] set_robust_list(0x7fe49186fa20, 24 <unfinished ...>
[pid 63413] <... set_robust_list resumed>) = 0 <0.000165>
[pid 63413] clock_gettime(CLOCK_PROCESS_CPUTIME_ID, {tv_sec=0, tv_nsec=134605}) = 0 <0.000039>
[pid 63413] sched_setscheduler(0, SCHED_RR, [30]) = 0 <0.000039>
[pid 63413] clock_gettime(CLOCK_PROCESS_CPUTIME_ID, {tv_sec=1, tv_nsec=937745567}) = 0 <0.299343>
[pid 63413] newfstatat(1, "",  <unfinished ...>
[pid 63413] <... newfstatat resumed>{st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}, AT_EMPTY_PATH) = 0 <0.299883>
[pid 63413] getrandom( <unfinished ...>
[pid 63413] <... getrandom resumed>"\x14\xca\x53\xbe\xd5\xe3\x8d\xc7", 8, GRND_NONBLOCK) = 8 <0.345736>
[pid 63413] brk(NULL <unfinished ...>
[pid 63413] <... brk resumed>)          = 0x9de000 <0.199633>
[pid 63413] brk(0x9ff000 <unfinished ...>
[pid 63413] <... brk resumed>)          = 0x9ff000 <0.304243>
[pid 63413] write(1, "Loop time: 1.937611 seconds\n", 28Loop time: 1.937611 seconds
[pid 63413] <... write resumed>)        = 28 <0.200065>
[pid 63413] exit_group(0 <unfinished ...>
[pid 63413] <... exit_group resumed>)   = ?
[pid 63413] +++ exited with 0 +++
[pid 63411] <... wait4 resumed>NULL, 0, NULL) = 63413 <0.617671>
[pid 63411] --- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=63413, si_uid=0, si_status=0, si_utime=193, si_stime=0} ---
```

Program-3-multi má teda problém s plánovaním procesov, ktorý je spôsobený zlým nastavením masky CPU vlákien (povolením iba 1 vlákna) a zmenou nastavenia plánovača procesov v child procesoch počas merania a nie pred meraním. Ďalšou možnosťou ako vyriešiť problém s obmedzením na 1 vlákno je zmeniť volanie vytvárania child procesov z `clone` na `fork`, ktoré nekopíruje masku CPU vlákien a umožní procesu bežať na viacerých CPU vláknach.

V porovnaní volaní z `strace` s program-3-single, program-3-multi taktiež v parent procese volá medzi vytváraním jednotlivých child procesov `clock_nanosleep(CLOCK_REALTIME, 0, {tv_sec=1, tv_nsec=0}, ...)` - uspanie procesu na 1s, tieto volania môžu ovplyvniť prepínanie procesov a taktiež spomaliť celkové trvanie programu. 

---

#### "Zákazník"/"Klient" #4:
*Niečo nám na našom systéme zkonzumovalo veľke množstvo pamäte, ale naše aplikácie to nie sú. Prosím pomôžte nám identifikovať, čo tú pamäť alokuje.*

*(Pozn.: Problematické prostredie vytvoríte spustením "program-4-setup". Následne môžete vyčistiť alokovanú pamäť spustením "program-4-cleanup". .. Je nutné explicitne dokázať, čo pamäť alokovalo. Vysvetliť len "čo robí program-4" nie je postačujúce!)*

Vypíšeme si slabinfo před a po zapnutí problematického prostředí a podíváme se na diff:
```
cat /proc/slabinfo > slabinfo_before.log
./program-4-setup
cat /proc/slabinfo > slabinfo_after.log

diff slabinfo_before.log slabinfo_after.log
```

Za povšimnutí stojí řádky ohledně slabů kmalloc-2k a kmalloc-32, které razantně nabobtnaly po zapnutí prostředí.

Pomocí slab_debug možná lze něco zjistit, lze zapnout ladění pro jednotlivé druhy pomocí složek v `/sys/kernel/debug/slab/<SLAB>`.
Zapneme pomocí kernelového parametru `slub_debug=U` pro sledování, kdo alokoval paměť.
Restartujeme systém a v `/sys/kernel/debug/slab/<kmalloc-32>|<kmalloc-2k>/alloc_traces` najdeme:

Pro kmalloc-2k:
```
 262144 allocate_treasury+0x8d/0x186 [pr4_kmod] age=7049/7207/7371 pid=1501 cpus=1
```

Pro kmalloc-32:
```
262142 allocate_treasury+0x5f/0x186 [pr4_kmod] age=2544/2813/3085 pid=1502 cpus=0
16384 allocate_treasury+0xf2/0x186 [pr4_kmod] age=2500/2522/2544 pid=1502 cpus=0
```

Alokace provádí funkce `allocate_treasury()` z kernelového modulu `pr4_kmod`.
