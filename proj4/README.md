# Tracing and profiling

Ku zadanie máte priložených niekoľko spustiteľných súborov, ktoré reprezentujú 4 oddelené príklady.

V tomto projekte dostanete od virtuálneho "zákazníka"/"klienta" popis problému s ich programom. Vašou úlohou bude použiť dostupné tracing nástroje aby Ste vedeli odpovedať na "zákazníkov"/"klientov" dotaz, prípadne vysvetliť "kde" a "čo" je problém. 
Samozrejme svoje tvrdenia musíte mať podložené datami, ktoré ukážete, vysvetlíte implikáciu Vášho záveru a tiež presne a exaktne popíšete ako Ste ich získali (aby si to "zákazník"/"klient" mohol overiť).

Pozn.: Je viacero spôsobov, ako sa dopracovať ku správnemu záveru. Odporúčam však nerobiť reverse-engineering pomocou nástrojou ako `gdb` a pod. Každopádne, ak budete mať správny záver a dostatočne dôkazné data, body dostanete.

Primárne sa zamerajte na prácu s nástrojmi:
- strace
- perf
- trace-cmd (kernel tracing)
- systemtap / eBPF

Nezabudnite ani na rôzne informácie z kernelu:
- proc-fs, sys-fs, debug-fs, ...
- slub_debug, page_owner


### Zadanie problémov:

#### "Zákazník"/"Klient" #1:
Náš progam "program-1" zaťažuje jedno celé CPU, ale vačšina cpu-usage je %system. Viete nám vysvetliť prečo? Je to nejaká chyba v kernelu?

#### "Zákazník"/"Klient" #2:
Máme HPC (High-Performance Computing) program "program-2" a zaujímalo by nás, jak moc je jeho výkon (čas do ukončenia programu) degradovaný prerušeniami a prípadne inými asynchronnými systémovými a kernelovými rutinami. Viete nám pomôcť zmerať tento "system/kernel overhead"?

(Pozn.: Nemusíte nutne zahrnúť úplne všetky možné prerušenia a overhead, stačí ak vytrasujte a spočítate aspoň nejaký výrazný (napr. kernel timery). Hodnotiť sa bude hlavne spôsob ako ste ku svojmu záveru dospeli.)

#### "Zákazník"/"Klient" #3:
Máme problém s naším viac-procesovým (multithread) programom. Keď spustíme iba jedno vlákno "program-3-single", výkon (čas na spočítanie jedného cyklu) je podstatne lepší než keď spustíme viac-procesovú formu "program-3-multi". Pre porovnanie:
~~~
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
~~~
Naši developeri si myslia, že je problém niekde s plánovaním procesov. Viete nám pomôcť potvrdiť túto hypotézu, prípadne zistit kde inde je problém a ako by sme ho mohli vyriešiť?

#### "Zákazník"/"Klient" #4:
Niečo nám na našom systéme zkonzumovalo veľke množstvo pamäte, ale naše aplikácie to nie sú. Prosím pomôžte nám identifikovať, čo tú pamäť alokuje.

(Pozn.: Problematické prostredie vytvoríte spustením "program-4-setup". Následne môžete vyčistiť alokovanú pamäť spustením "program-4-cleanup". .. Je nutné explicitne dokázať, čo pamäť alokovalo. Vysvetliť len "čo robí program-4" nie je postačujúce!)
