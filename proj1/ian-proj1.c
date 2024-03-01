/**
 * @file ian_proj1.c
 * @brief Prints ELF symbol table.
 * @author Marek Gergel (xgerge01)
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <elf.h>
#include <gelf.h>
#include <libelf.h>

/**
 * @brief Prints ELF symbol table.
 * @param elf ELF file
 */
void print_symbols(Elf *elf) {
    GElf_Shdr shdr;
    Elf_Scn *scn = NULL;
    Elf_Data *data;
    GElf_Sym sym;

    // Search all sections
    while ((scn = elf_nextscn(elf, scn))) {

        if (gelf_getshdr(scn, &shdr) != &shdr) continue;

        // Check if section contains symbols
        if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {

            if (!(data = elf_getdata(scn, NULL))) continue;
            
            int symbols = shdr.sh_size / shdr.sh_entsize;

            // Print the symbol table
            printf("%-20s %s\n", "Name", "Value");

            for (int i = 0; i < symbols; ++i) {

                gelf_getsym(data, i, &sym);
                
                if (GELF_ST_TYPE(sym.st_info) != STT_OBJECT) continue;
                
                if (sym.st_shndx == SHN_UNDEF || sym.st_shndx == SHN_COMMON || sym.st_shndx == SHN_ABS) continue;

                Elf_Scn *data_scn = elf_getscn(elf, sym.st_shndx);
                if (!data_scn) continue;
                
                Elf_Data *data_data = elf_getdata(data_scn, NULL);
                if (!data_data) continue;

                printf("%-20s %d\n", elf_strptr(elf, shdr.sh_link, sym.st_name), (int)(((int*)data_data->d_buf)[sym.st_value/sizeof (int)]));
            }
        }
    }
}

/**
 * @brief Main function.
 * @param argc Number of arguments
 * @param argv Arguments
 * @return 0 if success, anything else if error
 */
int main(int argc, char *argv[]) {

    if (argc != 2) {
        errx(1, "Usage: ian_proj1 FILE");
    }

    int fd = open(argv[1], O_RDONLY, 0);
    if (fd < 0) {
        errx(1, "open() failed");
    }

    if (elf_version(EV_CURRENT) == EV_NONE) {
        errx(elf_errno(), "ELF library initialization failed: %s", elf_errmsg(-1));
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (elf == NULL) {
        errx(elf_errno(), "elf_begin() failed: %s", elf_errmsg(-1));
    }

    print_symbols(elf);

    elf_end(elf);
    close(fd);

    return 0;
}
