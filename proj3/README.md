# Crash advanced analysis 

Ku zadaniu máte priložený súbor ‘vmcore_p3’, ktorý budete analyzovať. Je to vmcore vytvorený ako dôsledok kernel panic verzie 4.18.0-348.12.2.el8_5 na architektúre x86_64.

Podobne ako u projektu-2 bude Vaša úloha zistiť a presne popísať stav systému v čase panic a čo a ako viedlo ku samotnému panic.

<!> Je jasné, že vmcore_p3 je vytvorený "manuálne" pomocou kernel modulu, a že teda ultimátny dôvod kernel panic je práve "to" čo spravil tento modul. Váše riešenie má popísať čo konkrétne tento modul spravil a jak konkrétne to súvisí s dôvodom prečo panic nastal.   

<!> Ak budú predmetom Vašej analýzy nejaké kernel zámky (spinlock/mutex/rw_sem/..), Vaše riešenie musí jasne ukázať presnú adresu zámku a jeho typ (ideálne si vypíšte aj celú štruktúru) a aj kontext k čomu daný zámok patrí, i.e. čo zamyká?

   (Pre identifikovanie "k čomu zámok patrí / čo zamyká" je vhodné nahliadnuť pre kontext do zdrojového kódu kernelu. Nájdete ho v `home` adresároch na vašich VM, prípadne si ho stiahnite sami - je to OpenSource)

Svoje riešenie vypracujte tiež podobne ako u projektu-2 "voľnou odpoveďou" tak, aby čitateľ znalý crash analýzy z Vášej odpovede jasne pochopil Váš záver aj postup. 

Inšpiratívna osnova:
- V akom stave bol všeobecne systém? (Všimli Ste si nejakých vlastností, ktoré je vhodné spomenúť?)
- Prečo kernel zpanikáril? (Popíšte presný konkrétny dôvod, prečo kernel volal funkciu panic().)
- Čo spôsobilo situáciu, ktorá viedla ku kernel panic? 
  (Áno, bol to kernel modul. Popíšte "čo spravil" a ako to viedlo ku danej kernel panic.)
- Prečo daná situácia nastala?
   <!> Jasne ukážte adresy relevantných zámkov a k čomu patria. 
- Dalo by sa podobnému kernel panic vyhnúť, prípadne ako?
