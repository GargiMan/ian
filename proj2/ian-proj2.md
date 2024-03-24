### IAN - Projekt 2
Marek Gergel (xgerge01)

---

    $ crash <vmlinux> <vmcore>
          KERNEL: vmlinux
        DUMPFILE: vmcore_p2  [PARTIAL DUMP]
            CPUS: 4
            DATE: Wed Mar 23 15:02:26 2022
          UPTIME: 04:50:50
    LOAD AVERAGE: 0.00, 0.00, 0.00
           TASKS: 186
        NODENAME: rhel8-student-temp
         RELEASE: 4.18.0-348.12.2.el8_5.x86_64
         VERSION: #1 SMP Mon Jan 17 07:06:06 EST 2022
         MACHINE: x86_64  (2095 Mhz)
          MEMORY: 2 GB
           PANIC: "Kernel panic - not syncing: Out of memory: system-wide panic_on_oom is enabled"
             PID: 2359
         COMMAND: "stress"
            TASK: ff38963f46e65ac0  [THREAD_INFO: ff38963f46e65ac0]
             CPU: 0
           STATE: TASK_RUNNING (PANIC)

Z úvodných informácii po načítaní crash dump súboru sa dozvieme, že kernel zpanikáril počas behu príkazu stress kvôli nedostatku pamäte. Systém bol v stave, kedy bola využitá takmer celá pamäť a panic_on_oom bol zapnutý.

---

    crash> bt
    PID: 2359   TASK: ff38963f46e65ac0  CPU: 0   COMMAND: "stress"
     #0 [ff41a7a440acfa38] machine_kexec at ffffffff9e6635ce
     #1 [ff41a7a440acfa90] __crash_kexec at ffffffff9e79d6bd
     #2 [ff41a7a440acfb58] panic at ffffffff9e6eb227
     #3 [ff41a7a440acfbd8] out_of_memory.cold.35 at ffffffff9e87e6f1
     #4 [ff41a7a440acfc18] __alloc_pages_slowpath at ffffffff9e8d4825
     #5 [ff41a7a440acfd10] __alloc_pages_nodemask at ffffffff9e8d4beb
     #6 [ff41a7a440acfd70] alloc_pages_vma at ffffffff9e8ef414
     #7 [ff41a7a440acfdb0] do_anonymous_page at ffffffff9e8b1077
     #8 [ff41a7a440acfde8] __handle_mm_fault at ffffffff9e8b7336
     #9 [ff41a7a440acfe98] handle_mm_fault at ffffffff9e8b742e
    #10 [ff41a7a440acfec0] __do_page_fault at ffffffff9e674f5d
    #11 [ff41a7a440acff20] do_page_fault at ffffffff9e675267
    #12 [ff41a7a440acff50] page_fault at ffffffff9f00111e
        RIP: 000056525428d210  RSP: 00007ffe41cb9f40  RFLAGS: 00010206
        RAX: 000000000841a000  RBX: 0000000000000000  RCX: 00007fb13e034010
        RDX: 0000000000000000  RSI: 0000000010001000  RDI: 0000000000000000
        RBP: 00007fb13e034010   R8: 00000000ffffffff   R9: 0000000000000000
        R10: 0000000000000022  R11: 0000000000000246  R12: 0000000000001000
        R13: 000056525428f004  R14: 0000000000000002  R15: 0000000010000000
        ORIG_RAX: ffffffffffffffff  CS: 0033  SS: 002b

Z backtrace príkazu vyplýva, že kernel sa pokúsil alokovať stránky, ale nedokázal ich alokovať pomalou ani rýchlou cestou, kvôli nedostatku pamäte. Vzhľadom na zapnutú panic_on_oom funkciu, kernel zvolil panic.

V prípade, že by táto funkcia nebola zapnutá, kernel by zabil proces, ktorý by uvolnil potrebnú pamäť.

---

    crash> log
    bash (2287): drop_caches: 3
    stress invoked oom-killer: gfp_mask=0x6280ca(GFP_HIGHUSER_MOVABLE|__GFP_ZERO), order=0, oom_score_adj=0
    ... 
    Mem-Info:
    active_anon:192 inactive_anon:420327 isolated_anon:0
                active_file:94 inactive_file:21 isolated_file:0
                unevictable:0 dirty:0 writeback:0
                slab_reclaimable:5981 slab_unreclaimable:9690
                mapped:431 shmem:2185 pagetables:2570 bounce:0
                free:12961 free_pcp:93 free_cma:0
    Node 0 active_anon:768kB inactive_anon:1681308kB active_file:376kB inactive_file:84kB unevictable:0kB isolated(anon):0kB isolated(file):0kB mapped:1724kB dirty:0kB writeback:0kB shmem:8740kB shmem_thp: 0kB shmem_pmdmapped: 0kB anon_thp: 1454080kB writeback_tmp:0kB kernel_stack:2928kB pagetables:10280kB all_unreclaimable? yes
    Node 0 DMA free:7280kB min:384kB low:480kB high:576kB active_anon:0kB inactive_anon:7824kB active_file:0kB inactive_file:0kB unevictable:0kB writepending:0kB present:15992kB managed:15360kB mlocked:0kB bounce:0kB free_pcp:0kB local_pcp:0kB free_cma:0kB
    lowmem_reserve[]: 0 1730 1730 1730 1730
    Node 0 DMA32 free:44564kB min:44668kB low:55832kB high:66996kB active_anon:768kB inactive_anon:1673668kB active_file:1052kB inactive_file:188kB unevictable:0kB writepending:0kB present:2080628kB managed:1842672kB mlocked:0kB bounce:0kB free_pcp:372kB local_pcp:348kB free_cma:0kB
    lowmem_reserve[]: 0 0 0 0 0
    Node 0 DMA: 1*4kB (M) 1*8kB (M) 0*16kB 0*32kB 1*64kB (M) 0*128kB 2*256kB (UM) 1*512kB (U) 0*1024kB 1*2048kB (M) 1*4096kB (M) = 7244kB
    Node 0 DMA32: 516*4kB (UME) 539*8kB (UME) 272*16kB (UME) 80*32kB (UME) 37*64kB (UME) 21*128kB (UME) 5*256kB (UE) 6*512kB (UE) 22*1024kB (UM) 0*2048kB 0*4096kB = 45224kB
    Node 0 hugepages_total=0 hugepages_free=0 hugepages_surp=0 hugepages_size=1048576kB
    Node 0 hugepages_total=0 hugepages_free=0 hugepages_surp=0 hugepages_size=2048kB
    2265 total pagecache pages
    0 pages in swap cache
    Swap cache stats: add 0, delete 0, find 0/0
    Free swap  = 0kB
    Total swap = 0kB
    524155 pages RAM
    0 pages HighMem/MovableOnly
    59647 pages reserved
    0 pages hwpoisoned
    Tasks state (memory values in pages):
    [  pid  ]   uid  tgid total_vm      rss pgtables_bytes swapents oom_score_adj name
    [    701]     0   701    22387     1024   237568        0             0 systemd-journal
    [    729]     0   729    29401     1088   233472        0         -1000 systemd-udevd
    [    875]    32   875    16800      182   176128        0             0 rpcbind
    [    876]     0   876    37686      171   155648        0         -1000 auditd
    [    878]     0   878    12140       77   131072        0             0 sedispatch
    [    939]     0   939    17366      256   180224        0             0 qemu-ga
    [    940]   998   940   441353     1740   368640        0             0 polkitd
    [    941]    81   941    16151      210   163840        0          -900 dbus-daemon
    [    942]     0   942   107224      523   454656        0             0 sssd
    [    944]     0   944    31247      159   147456        0             0 irqbalance
    [    949]   992   949    37789      177   188416        0             0 chronyd
    [    981]     0   981   109381      702   483328        0             0 sssd_be
    [    986]     0   986   109754      451   491520        0             0 sssd_nss
    [    989]     0   989    22996      623   208896        0             0 systemd-logind
    [   1021]     0  1021   152425      650   417792        0             0 NetworkManager
    [   1026]     0  1026   176738     4021   454656        0             0 tuned
    [   1029]     0  1029    62887      135   114688        0             0 rhsmcertd
    [   1034]     0  1034    77892      165   188416        0             0 gssproxy
    [   1238]     0  1238    23087      234   200704        0         -1000 sshd
    [   1239]     0  1239    54637      588   200704        0             0 rsyslogd
    [   1289]     0  1289    56588       28    65536        0             0 agetty
    [   1291]     0  1291    56498       29    86016        0             0 agetty
    [   1294]     0  1294    61679      218   114688        0             0 crond
    [   2236]     0  2236    38361      315   307200        0             0 sshd
    [   2243]  1001  2243    22362      337   208896        0             0 systemd
    [   2247]  1001  2247    77734     1572   307200        0             0 (sd-pam)
    [   2254]  1001  2254    38361      316   290816        0             0 sshd
    [   2255]  1001  2255    58478      133    94208        0             0 bash
    [   2282]     0  2282    87390      259   290816        0             0 sudo
    [   2286]     0  2286    84256      227   274432        0             0 su
    [   2287]     0  2287    59042      184    94208        0             0 bash
    [   2348]     0  2348     1996       22    57344        0             0 stress
    [   2349]     0  2349   133069   131047  1110016        0             0 stress
    [   2350]     0  2350   133069   131047  1110016        0             0 stress
    [   2358]     0  2358     1996       22    53248        0             0 stress
    [   2359]     0  2359    67533    33839   327680        0             0 stress
    [   2360]     0  2360    67533    21235   225280        0             0 stress
    [   2361]     0  2361    67533    42605   397312        0             0 stress
    [   2362]     0  2362    67533    41351   385024        0             0 stress

Z logu sa dozvieme, že systém bol v stave zaťaženia pamäte pomocou utility stress.
Týchto procesov bolo viacero a využívali veľké množstvo pamäte. 

---

    crash> ps
    PID    PPID  CPU        TASK       ST  %MEM     VSZ    RSS  COMM 
    ...
    2287   2286   3  ff38963f418bdac0  IN   0.0  236168    736  bash
    2348   2287   1  ff38963f73d28000  IN   0.0    7984    104  stress
    2349   2348   2  ff38963f46c00000  IN  25.0  532276 524388  stress
    2350   2348   1  ff38963f48800000  IN  25.0  532276 524388  stress
    2358   2287   1  ff38963f73d29e40  IN   0.0    7984    104  stress
    2359   2358   0  ff38963f46e65ac0  RU   6.5  270132 135372  stress
    2360   2358   2  ff38963f41920000  UN   4.1  270132  85008  stress
    2361   2358   2  ff38963f54a35ac0  RU   8.1  270132 170676  stress
    2362   2358   1  ff38963f419d5ac0  RU   7.9  270132 165616  stress

Vo výpise procesov je zrejmé, že procesy stress využívali viac ako 75% celkovej dostupnej pamäte. V stĺpci RSS sú aj konkrétne hodnoty využitej fyzickej pamäte. 
Hodnoty procesov stress v stĺpci VSZ majú celkovú hodnotu cez 2GB vyžadovanej pamäte, čo je viac ako celková dostupná pamäť systému uvedená v úvodných informáciách.

---

    crash> kmem -i
                     PAGES        TOTAL      PERCENTAGE
        TOTAL MEM   464508       1.8 GB         ----
             FREE    12961      50.6 MB    2% of TOTAL MEM
             USED   451547       1.7 GB   97% of TOTAL MEM
           SHARED     1615       6.3 MB    0% of TOTAL MEM
          BUFFERS        0            0    0% of TOTAL MEM
           CACHED     2265       8.8 MB    0% of TOTAL MEM
             SLAB     8339      32.6 MB    1% of TOTAL MEM

       TOTAL HUGE        0            0         ----
        HUGE FREE        0            0    0% of TOTAL HUGE
    
       TOTAL SWAP        0            0         ----
        SWAP USED        0            0    0% of TOTAL SWAP
        SWAP FREE        0            0    0% of TOTAL SWAP
    
     COMMIT LIMIT   232254     907.2 MB         ----
        COMMITTED   607729       2.3 GB  261% of TOTAL LIMIT

V súhrnom výpise pamäte je uvedené, že systém mal dostupných celkovo 1.8 GB pamäte (z 2GB písaných v úvode), z čoho bolo voľných len 50.6 MB a systém nemal dostupnú žiadnu swap pamäť. V zavere 'COMMITED' je uvedené, že systém prekročil limit alokovaných stránok o 261%.

Tomuto stavu sa dalo predísť napríklad zvýšením pamäte systému, pridaním swap pamäte alebo zvýšením limitu alokovaných stránok.