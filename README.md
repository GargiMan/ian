# ian

### Project evaluation:

| Project | Score |
|---------|-------|
| proj1   | 8/15  | 
| proj2   | 10/10 |
| proj3   | 9/20  |
| proj4   | 13/15 |

#### proj1 notes:
all issues fixed after evaluation

- Basic object file: 2 (max 2)
- Basic executable file: 0 (max 2) 
- Negative value: 2 (max 2) 
- Uninitialized symbols: 0 (max 2) 
- Symbols in custom section: 2 (max 2) 
- Extern symbols: 1 (max 1) 
- No spurious symbols: 1 (max 1) 
- No symbol table: 2 (max 2) 
- No an ELF file/empty file: 0 (max 1) 
- SIGSEGV: -2 

#### proj3 notes:
- Nie je vysvetlene, ze je zapnuty hung_task_panic (default je ‘0’). 
- Nie je ukazana spravna adresa PID 1 mmap_sem. 
- Nie je ukazane, ze bash PID 1706 naozaj zamkol PID 1 mmap_sem. 
- Nie je navrhnute ako riesit alebo predchadzat takejto situacii. … Adresa rw_sem sa da vytiahnut aj zo stacku PID 1, pripadne z task_struct->mm_struct PID 1 po tom co zistis zo zdrojaku page_fault ze sa jedna o mmap_sem current tasku. Ze PID 1706 naozaj zamkol dany mmap_sem sa da ukazat aj pomocou rw_sem->owner.

#### proj4 notes:
- PR2: Ono aj na tych par millisekundach moze zalezat -- pre priklad: taketo HPC, low latency workloady pouzivaju napriklad burzy a hlavne systemy co robia medziburzovu arbitraz – tam ked sa im iba za 1 millizekundu pohne cena len o 0.01% ale vo vysledku to moze znamenat rozdiel v tisicoch dolarov – to uplne nechces aby ti kernel interrupty kazili obchody.
- PR3: Aj fork aj clone dedia cpu affinity masku, ale prave u clone sa da explicitne specifikovat ina; fork je v tomto primitivnejsi.
- PR4: Nespocital si kolko pamate modul alokuje – vsimol by si si ze alokuje celkovo cca 1.5 GB, zatial co kmalloc alokuje v slaboch iba cca 0.5 GB, takze ti stale 1 GB chyba – tie totiz ten modul alokuje priamo cez alloc_pages a nerobi ziadny accounting, preto tie stranky proste “zmiznu” zo statistik – toto sa da sledovat bud tracovanim alokovacich funkcii v jadre alebo pomocou page_owner. 
- Pozn.: Extra poznamka u prikladu 2 dopadla bohuzial trochu zavadzajuca a mnohi z vas nezmerali ste, ze je program-2 bezne preplanovany na iny process (ci uz kernel workre alebo rozne service processy – nedeterministicky chaos). Kedze bola poznamka v zadani zbytocne zavadzajuca, tak nepritomnost tychto dat/faktov nehodnotim, avsak ti ktori zmerali aj tento overhead naopak ziskali bodovy bonus. Chcem na to epxlicitne poukazat, pretoze preplanovavanie na ine procesy tvori dost vyrazny overhead, obzvlast ked sa jedna o HPC workloady. (v prostrediach pre taketo HPC workloady sa aplikuju nastvenia ktore epxlicitne odstranuju jak prerusenia, tak preplanovavanie na ine procesy – ciel je aby HPC proces nebol vobec nicim ruseny a mal pre svoju pracu co najvacsi podiel cpu cyklov).