# ian

### Project evaluation:

| Project | Score |
|---------|-------|
| proj1   | 8/15  | 
| proj2   | 10/10 |
| proj3   | 9/20  |
| proj4   | ?/15  |

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
