### IAN - Projekt 3
Marek Gergel (xgerge01)

---
    $ crash <vmlinux> <vmcore>
          KERNEL: vmlinux  [TAINTED]
        DUMPFILE: vmcore_p3  [PARTIAL DUMP]
            CPUS: 4
            DATE: Wed Mar 30 15:34:31 CEST 2022
          UPTIME: 00:04:06
    LOAD AVERAGE: 2.54, 1.11, 0.43
           TASKS: 197
        NODENAME: rhel8-student-temp
         RELEASE: 4.18.0-348.12.2.el8_5.x86_64
         VERSION: #1 SMP Mon Jan 17 07:06:06 EST 2022
         MACHINE: x86_64  (2095 Mhz)
          MEMORY: 2 GB
           PANIC: "Kernel panic - not syncing: hung_task: blocked tasks"
             PID: 42
         COMMAND: "khungtaskd"
            TASK: ff47e40b7fac5ac0  [THREAD_INFO: ff47e40b7fac5ac0]
             CPU: 2
           STATE: TASK_RUNNING (PANIC)

Při spuštění nástroje `crash` se dovídáme, že panika nastala z důvodu blokovaných tasků.

---

Z logu se dovídáme, že blokovaná úloha byla systemd a před panikou byl vložen neznámý modul `smajdalf`.

    [   51.555322] smajdalf: loading out-of-tree module taints kernel.
    [   51.555903] smajdalf: module license 'RH-EDU' taints kernel.
    [   51.556426] Disabling lock debugging due to kernel taint
    [   51.556949] smajdalf: module verification failed: signature and/or required key missing - tainting kernel
    [   51.558281] Smajdalf: Carodej nikdy nechodi pozde.
    [  245.808706] INFO: task systemd:1 blocked for more than 120 seconds.
    [  245.809290]       Tainted: P           OE    --------- -  - 4.18.0-348.12.2.el8_5.x86_64 #1
    [  245.810057] "echo 0 > /proc/sys/kernel/hung_task_timeout_secs" disables this message.
    [  245.810779] task:systemd         state:D stack:    0 pid:    1 ppid:     0 flags:0x00000000

Příznak `Tainted: POE` značí, že načtený modul je unsigned a out-of-tree, tedy nebyl součástí jádra a tím pádem je podezřelý.

---

Dále přepneme kontext na proces systemd (PID 1) a vypíšeme backtrace.

    crash> set 1
    crash> bt
    PID: 1        TASK: ff47e40b01891e40  CPU: 2    COMMAND: "systemd"
    #0 [ff7ea32980327dc0] __schedule at ffffffff8a77a2ad
    #1 [ff7ea32980327e18] schedule at ffffffff8a77a787
    #2 [ff7ea32980327e28] rwsem_down_read_slowpath at ffffffff8a77d320
    #3 [ff7ea32980327ec0] __do_page_fault at ffffffff89e75121
    #4 [ff7ea32980327f20] do_page_fault at ffffffff89e75267
    #5 [ff7ea32980327f50] page_fault at ffffffff8a80111e
        RIP: 00007f2411c4e27f  RSP: 00007fff58f62300  RFLAGS: 00010206
        RAX: 000055c2b6810bf0  RBX: 00007f2411f89bc0  RCX: 000055c2b67f67c0
        RDX: 00007f2411f89c50  RSI: 00007f2411f89c40  RDI: 00007f2411f89bc8
        RBP: 000000000000001d   R8: 000055c2b66515f0   R9: 000055c2b66515e0
        R10: 00007f2411f89bc0  R11: 0000000000000007  R12: 0000000000000003
        R13: 000000000000001d  R14: 00007f2411f89bc0  R15: 0000000000000030
        ORIG_RAX: ffffffffffffffff  CS: 0033  SS: 002b

Proces systemd pravděpodobně čeká na semaforu ( dle volání `rwsem_down_read_slowpath()`) a proto je blokován.

---

Vyhledáme informace o symbolech v modulu `smajdalf`:

    crash> sym -m smajdalf
    ffffffffc0922000 MODULE START: smajdalf
    ffffffffc0922000 (t) take_the_lock_of_power
    ffffffffc0922060 (t) trik_se_spicatym_kloboukem
    ffffffffc092207d (t) smajdalf_cleanup
    ffffffffc092207d (t) cleanup_module
    ffffffffc0924000 (d) smajdalf_dir_table
    ffffffffc0924080 (d) smajdalf_table
    ffffffffc0924100 (d) magic_mutex
    ffffffffc0924140 (d) __this_module
    ffffffffc09244c0 (b) smajdalf_sysctl_header
    ffffffffc0926000 MODULE END: smajdalf

Modul obsahuje podezrelé funkce `take_the_lock_of_power`, `trik_se_spicatym_kloboukem` a také obsahuje proměnnou `magic_mutex`.

---

Assembly kód funkce `take_the_lock_of_power`:

    crash> dis ffffffffc0922000
    0xffffffffc0922000 <take_the_lock_of_power>:    nopl   0x0(%rax,%rax,1) [ftrace nop]
    0xffffffffc0922005 <take_the_lock_of_power+5>:  mov    -0x35508fcc(%rip),%rax                         # 0xffffffff8b419040 <init_task+2048>
    0xffffffffc092200c <take_the_lock_of_power+12>: push   %rbx                                           # Uchování hodnoty v registru rbx
    0xffffffffc092200d <take_the_lock_of_power+13>: cmp    $0xffffffff8b419040,%rax                       # Je v rax hodnota init_task+2048?
    0xffffffffc0922013 <take_the_lock_of_power+19>: je     0xffffffffc092201e <take_the_lock_of_power+30>
    0xffffffffc0922015 <take_the_lock_of_power+21>: cmpl   $0x1,0x100(%rax)                               # Je v paměti na adrese rax+0x100 hodnota 1?
    0xffffffffc092201c <take_the_lock_of_power+28>: je     0xffffffffc092204c <take_the_lock_of_power+76>
    0xffffffffc092201e <take_the_lock_of_power+30>: mov    $0x9502f8ff,%ebx                               # Hodnota pro čítač do ebx
    0xffffffffc0922023 <take_the_lock_of_power+35>: pause                                                 # spinloop hint pro CPU
    0xffffffffc0922025 <take_the_lock_of_power+37>: call   0xffffffff8a77a850 <_cond_resched>             # Přeplánování
    0xffffffffc092202a <take_the_lock_of_power+42>: sub    $0x1,%rbx                                      # Dekrementace čítače
    0xffffffffc092202e <take_the_lock_of_power+46>: jne    0xffffffffc0922023 <take_the_lock_of_power+35> # Dokud se instrukce od pause neprovedou ebx-krát.
    0xffffffffc0922030 <take_the_lock_of_power+48>: mov    $0xffffffffc0924100,%rdi                       # Do rdi se se vloží adresa "magic_mutex"
    0xffffffffc0922037 <take_the_lock_of_power+55>: call   0xffffffff8a77c9c0 <mutex_lock>                # Zamkne se tento mutex
    0xffffffffc092203c <take_the_lock_of_power+60>: mov    $0xffffffffc0924100,%rdi                       # Znovu se do rdi vloží adresa stejného (zamknutého) mutexu
    0xffffffffc0922043 <take_the_lock_of_power+67>: call   0xffffffff8a77c9c0 <mutex_lock>                # A opět volání, aby se zamknul
    0xffffffffc0922048 <take_the_lock_of_power+72>: xor    %eax,%eax                                      # Vynulováni eax (return kód 0)
    0xffffffffc092204a <take_the_lock_of_power+74>: pop    %rbx                                           # Obnovení hodnoty, co byla v rbx
    0xffffffffc092204b <take_the_lock_of_power+75>: ret                                                   # návrat z funkce
    0xffffffffc092204c <take_the_lock_of_power+76>: mov    0x50(%rax),%rdi                                # Do rdi se vloží hodnota z paměti na adrese rax+0x50
    0xffffffffc0922050 <take_the_lock_of_power+80>: add    $0x70,%rdi                                     # K této hodnotě se přičte 0x70
    0xffffffffc0922054 <take_the_lock_of_power+84>: call   0xffffffff8a77cf30 <down_write>                # Zamkne semafor k zápisu
    0xffffffffc0922059 <take_the_lock_of_power+89>: jmp    0xffffffffc092201e <take_the_lock_of_power+30>

Druhý řádek kopíruje do registru `rax` hodnotu pomocí offsetu.
Crash napovídá, že daná adresa ukazuje na struct `init_task` s offsetem 2048.
Což je [proměnná](https://elixir.bootlin.com/linux/v4.18/source/include/linux/sched/task.h#L26) reprezentující první úlohu (task), která se inicializuje při startu kernelu.

`pahole task_struct vmlinuz.xz`

        /* --- cacheline 32 boundary (2048 bytes) --- */
    struct sysv_sem            sysvsem;              /*  2048     8 */
    struct sysv_shm            sysvshm;              /*  2056    16 */

Pomocí programu `pahole` lze zjistit, že na offsetu 2048 do této struktury leží položka `sysvsem`,
což je ukazatel na [strukturu](https://elixir.bootlin.com/linux/v4.18/source/include/linux/sem.h#L12) obsahující
[strukturu](https://elixir.bootlin.com/linux/v4.18/source/ipc/sem.c#L161) `sem_undo_list`, která ovládá přístup k seznamu struktur `sem_undo`.

Hodnota proměnné `sysvsem` v `init_task` v coredumpu je 0x0.

---

Další analýza funkce `take_the_lock_of_power` pomocí pseudokódu:

```c
    if (rax != init_task+2048) {
        if ([rax+0x100] == 1) {
            rdi = [rax + 0x50] + 70;
            down_write(rdi); // tady už se nedozvím, co je v rax, rdi
        }
    }

    for(int i = 0x9502f8ff; i > 0; i--) {
        _cond_resched();
    }

    rdi = magic_mutex;
    mutex_lock(rdi);

    rdi = magic_mutex;
    mutex_lock(rdi);

    return 0;
```

Můžeme zjistit něco o funkcích, jaké jsou volány a s jakými parametry.

Instrukce `call` v `take_the_lock_of_power+84` volá funkci [`down_write()`](https://elixir.bootlin.com/linux/v4.18/source/kernel/locking/rwsem.c#L67),
která bere ukazatel na semafor a zamyká jej k zápisu. Naneštěstí se k hodnotě předaného ukazatele nedostaneme, jelikož tato větev kódu se provede
v případě, že v rax není init_task+2048, ale něco jiného. Pomocí této neznámé hodnoty se poté vypočítá předaný ukazatel.

---

Assembly kód funkce `trik_se_spicatym_kloboukem`:

    crash> dis ffffffffc0922060    
    0xffffffffc0922060 <trik_se_spicatym_kloboukem>:        nopl   0x0(%rax,%rax,1) [FTRACE NOP]
    0xffffffffc0922065 <trik_se_spicatym_kloboukem+5>:      mov    $0xffffffffc0924100,%rdi
    0xffffffffc092206c <trik_se_spicatym_kloboukem+12>:     callq  0xffffffff8a77c9c0 <mutex_lock>
    0xffffffffc0922071 <trik_se_spicatym_kloboukem+17>:     mov    $0xffffffffc0924100,%rdi
    0xffffffffc0922078 <trik_se_spicatym_kloboukem+24>:     jmpq   0xffffffff8a77c9c0 <mutex_lock>

Funkce `trik_se_spicatym_kloboukem` zamyká mutex `magic_mutex` dvakrát za sebou obdobně jako `take_the_lock_of_power`.

---

K mutexu, který je dvakrát po sobě zamknut se ale lze dostat. Předá se přes registr `rdi` do funkce
[`mutex_lock()`](https://elixir.bootlin.com/linux/v4.18/source/kernel/locking/mutex.c#L237) a jeho adresa je přímo daná: `0xffffffffc0924100`. 

```c
    crash> struct mutex 0xffffffffc0924100
    struct mutex {
        owner = {
            counter = -51822134688203647
        }, 
        wait_lock = {
            {
                rlock = {
                    raw_lock = {
                        {
                            val = {
                                counter = 0
                            }, 
                            {
                                locked = 0 '\000', 
                                pending = 0 '\000'
                            }, 
                            {
                                locked_pending = 0, 
                                tail = 0
                            }
                        }
                    }
                }
            }
        }, 
        osq = {
            tail = {
            counter = 0
            }
        }, 
        wait_list = {
            next = 0xff7ea32980ae7e00, 
            prev = 0xff7ea32980ae7e00
        }
    }
```

Prohledání hodnot této struktury nám ukáže `wait_list` tohoto mutexu obsahující jedinou hodnotu: `0xff7ea32980ae7e00`. 
Kmem nám tuto paměť asocijuje s procesem `bash`.

    crash> kmem 0xff7ea32980ae7e00
        PID: 1706
    COMMAND: "bash"
       TASK: ff47e40b06683c80  [THREAD_INFO: ff47e40b06683c80]
        CPU: 0
      STATE: TASK_UNINTERRUPTIBLE 

    VMAP_AREA         VM_STRUCT                 ADDRESS RANGE                SIZE
    ff47e40b0712c280  ff47e40b07f68ec0  ff7ea32980ae4000 - ff7ea32980ae9000    20480

        PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
    fffadfbe401ea0c0  7a83000                0        0  1 fffffc0000000

---

Z [dokumentace](https://www.kernel.org/doc/html/v5.3/locking/mutex-design.html) jsem se dočetl,
že položka `owner` obsahuje ukazatel na `task_struct` úlohy, která zámek drží. Dokumentace je sice
pro novější verze, ale když jsem to zkusil převést, byla to adresa task structu již zmíněného bashe.

-51822134688203647 = 0xFF47E40B06683C81

    crash> kmem FF47E40B06683C81 
    CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
    ff47e40b0101d6c0     7688        196       228     57    32k  task_struct
    SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
    fffadfbe4019a000  ff47e40b06680000     0      4          4     0
    FREE / [ALLOCATED]
    [ff47e40b06683c80]

        PID: 1706
    COMMAND: "bash"
       TASK: ff47e40b06683c80  [THREAD_INFO: ff47e40b06683c80]
        CPU: 0
      STATE: TASK_UNINTERRUPTIBLE 

        PAGE       PHYSICAL      MAPPING       INDEX CNT FLAGS
    fffadfbe4019a0c0  6683000 dead000000000400        0  0 fffffc0000000

Což dává smysl, funkce dvakrát za sebou zamknula mutex, tedy je vlastníkem i čekajícím. Došlo k deadlocku,
proces bude čekat donekonečna. Bash je to proto, že příkaz pro vložení modulu do kernelu byl pravděpodobně spuštěn z bashe.

---

### Závěr

Kernel zpanikařil, protože úloha systemd byla zablokována déle než předem určený limit.
Z kernelové backtraceu toho procesu můžeme vidět, že skončila na `rwsem_down_read_slowpath()`,
což byl pravděpodobně stejný semafor, jako ten neidentifikovatelný, který se objevil v disassebmlu funkcí `take_the_lock_of_power` a `trik_se_spicatym_kloboukem`.
Úloha, která držela tento semafor se poté pokusila dvakrát zamknout ten stejný mutex, což ji zablokovalo a semafor tedy nemohl být odemknut.
