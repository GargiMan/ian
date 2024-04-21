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
 * @param is_rel 1 if ELF is relocatable, 0 otherwise
 */
void print_symbols(Elf *elf, int is_rel) {
    GElf_Shdr shdr;
    Elf_Scn *scn = NULL;
    Elf_Data *data;
    GElf_Sym sym;
    GElf_Rel rel;
    int started = 0;

    // Search all sections
    while ((scn = elf_nextscn(elf, scn))) {

        if (gelf_getshdr(scn, &shdr) != &shdr) continue;

        // Check if section contains symbols
        if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {

            if (!(data = elf_getdata(scn, NULL))) continue;
            
            int symbols = shdr.sh_size / shdr.sh_entsize;

            for (int i = 0; i < symbols; ++i) {

                gelf_getsym(data, i, &sym);
                
                if (GELF_ST_TYPE(sym.st_info) != STT_OBJECT) continue;
                
                if (sym.st_shndx == SHN_UNDEF || sym.st_shndx == SHN_COMMON || sym.st_shndx == SHN_ABS) continue;

                if (sym.st_other != STV_DEFAULT) continue;

                if (sym.st_size == 0) continue;

                Elf_Scn *data_scn = elf_getscn(elf, sym.st_shndx);
                if (!data_scn) continue;
                
                GElf_Shdr data_shdr;
                gelf_getshdr(data_scn, &data_shdr);
                if (!&data_shdr) continue;

                Elf_Data *data_data = elf_getdata(data_scn, NULL);
                if (!data_data) continue;

                // Print the symbol table
                if (started == 0) {
                    printf("%-20s %s\n", "Name", "Value");
                    started = 1;
                }

                if (is_rel) {
                    if (data_data->d_buf) {
                        printf("%-20s %d\n", elf_strptr(elf, shdr.sh_link, sym.st_name), (int)(((int*)data_data->d_buf)[sym.st_value/sizeof(int)]));
                    } else {
                        printf("%-20s %s\n", elf_strptr(elf, shdr.sh_link, sym.st_name), "UNDEF");
                    }
                } else {  //TODO
                    Elf_Scn *rel_scn = elf_getscn(elf, shdr.sh_link);
                    if (!rel_scn) continue;

                    Elf_Data *rel_data = elf_getdata(rel_scn, NULL);
                    if (!rel_data) continue;

                    for (int j = 0; j < shdr.sh_size / shdr.sh_entsize; ++j) {
                        gelf_getrel(rel_data, j, &rel);
                        if (sym.st_value == GELF_R_SYM(rel.r_info) * sizeof(int)) {
                            printf("%-20s %d\n", elf_strptr(elf, shdr.sh_link, sym.st_name), (int)(((int*)data_data->d_buf)[rel.r_offset/sizeof(int)]));
                        }
                    }
                }
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
        close(fd);
        errx(elf_errno(), "ELF library initialization failed: %s", elf_errmsg(-1));
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (elf == NULL) {
        close(fd);
        errx(elf_errno(), "elf_begin() failed: %s", elf_errmsg(-1));
    }

    GElf_Ehdr ehdr;
    if (gelf_getehdr(elf, &ehdr) != &ehdr) {
        elf_end(elf);
        close(fd);
        errx(elf_errno(), "gelf_getehdr() failed: %s", elf_errmsg(-1));
    }

    print_symbols(elf, ehdr.e_type != ET_REL ? 0 : 1);

    elf_end(elf);
    close(fd);

    return 0;
}
