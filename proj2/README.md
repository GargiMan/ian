# Crash basics

Ku zadaniu máte priložený súbor ‘vmcore_p2’. Je to vmcore vytvorený ako dôsledok kernel panic verzie 4.18.0-348.12.2.el8_5 na architektúre x86_64.

Vaša úloha bude za pomoci programu ‘crash’ zistiť a následne popísať stav systému v čase panic, čo systém v danom momente robil a prečo kernel zpanikáril.

Program ‘crash’ potrebuje 2 argumenty: debug symboly a samotný vmcore (viz “man crash”). Debug symboly ('vmlinux') aj samotný vmcore máte zabalený v archíve 'ian_project2_2023.tar.gz'.

(Ak by Ste mali akýkoľvek problém s pustením crashu, alebo by Vám chýbali debug symboly, prosím dajte vedieť.)

Svoju prácu tvorte spôsobom voľnej odpovede, tzn. je len na Vás, ako projekt vypracujete. Dôležité je len, aby iný človek znalý vmcore analýzi, bol schopný z Vašej odpovede jasne pochopiť čo sa dialo a ako Ste ku svojim záverom dospeli. Ideáne vždy ukážte data (výpis z crash príkazu) vrátane samotného príkazu, ktorým Ste dané data vypísali, nasledované Vašim vysvetlením, čo Ste na základe daných dát zistili. Viz. abstraktný príklad nižšie.

Napríklad sa môžete (ale striktne nemusíte) držať nasledovnej osnovy, resp. odpovedať na nasledujúce otázky:
  *) V akom stave bol všeobecne systém? (Všimli Ste si nejakých vlastností, ktoré je vhodné spomenúť?)
  *) Prečo kernel zpanikáril?
  *) Čo spôsobilo situáciu, ktorá viedla ku kernel panic?
  *) Prečo daná situácia nastala?
  *) Dalo by sa podobnému kernel panic vyhnúť, prípadne ako?

(Pozn. vyššie uvedené otázky prosím nechápte ako “checklist”, ale ako inšpiráciu.)

V neposlednom rade ešte dodám, že je dôležité skontrolovať a vo Vašom riešení ukázať informácie relevantné ku dôvodu danej kernel panic.

V prípade akýchkoľvek nejasností sa nám neváhajte ozvať.

### Príklad formy písania projektu:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
]$ crash <vmlinux> <vmcore>
...

Hneď na začiatok sa dozvieme ...


crash> bt
...

Z backtracu vidíme, že ...


crash> log | less
...

Z logu sa dozvieme, že ...


crash> <command> [| grep | sort | awk | ...]
...

Pomocou príkazu `<command>` sme zistili, že ...

Zo zistených informácií vyplýva ...

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~