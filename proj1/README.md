Cílem projektu je seznámit se se způsobem práce s ELF soubory.  
Vašim úkolem je napsat program, který vypíše počáteční hodnoty globálních proměnných definovaných v programu. 

Pro zjednodušení:
- program může podporovat pouze 64-bitové ELF soubory
- program nemusí podporovat dynamické a nedefinované symboly
- stačí podporovat pouze proměnné typu int


Program vyžaduje právě jeden argument, čimž je jméno ELF souboru a nemusí podporovat žádné další přepínače.  
Spuštení programu je teda následující: `./ian-proj1 FILE`


Projekt implementujte v jazyce C a odevzdávejte jako TAR archiv s názvem `ian-proj1.tar.gz`, který obsahuje adresář obsahující všechny Vaše zdrojové (a případně hlavičkové) soubory a Makefile, který podporuje minimálně makra `all` pro překlad celého projektu a `clean` pro vyčištění adresáře.


Pro příklad mějme jednoduchý program:
```
#include <stdio.h>
int a = 10;
int b = 42;
int main() {
  printf("a=%d, b=%d\n", a, b);
  return 0;
}
```


Výstup spuštení Vašeho projektu nad objektovým ELF souborem vytvořeným z daného programu může být následující:
```
./ian-proj1 test.o
Name      Value
a         10
b         42
```

K řešení se Vám může hodit specifikace ELF formátu, případně jeho 64-bitového rozšíření, které naleznete zde:  
http://refspecs.linuxbase.org/elf/elf.pdf  
http://ftp.openwatcom.org/devel/docs/elf-64-gen.pdf  


Doporučujeme při implementaci využít nějakou knihovnu pro čtení formátu ELF, např. libelf, která je součástí projektu elfutils. Vaše distribuce Linuxu pravděpodobně elfutils nabízí mezi standardními balíčky. Existuje také implementace libelf pro jiné Unix a Unix-like systémy. Libelf také poskytuje hlavičkový soubor gelf.h, který poskytuje abstrakci nad 32-bit a 64-bit ELF soubory. Rovněž dáváme do pozornosti hlavičkový soubor elf.h, který je součástí glibc a který obsahuje definice všech maker standardu ELF.