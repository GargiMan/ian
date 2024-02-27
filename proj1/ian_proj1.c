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
    GElf_Ehdr ehdr;
    GElf_Shdr shdr;
    Elf_Scn *scn = NULL;
    Elf_Data *data;
    Elf32_Sym *sym32;
    Elf64_Sym *sym64;

    // Get the ELF header
    if (gelf_getehdr(elf, &ehdr) == NULL) {
        errx(elf_errno(), "gelf_getehdr() failed: %s\n", elf_errmsg(-1));
    }

    // Search all sections
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            continue;

        // Check if section contains symbol table
        if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {

            data = elf_getdata(scn, NULL);
            int symbols = shdr.sh_size / shdr.sh_entsize;

            if (data) {

                // Determine if symbols are 32 or 64 bit
                if (shdr.sh_entsize == sizeof(Elf32_Sym)) {
                    sym32 = (Elf32_Sym *) data->d_buf;
                    sym64 = NULL;
                } else {
                    sym32 = NULL;
                    sym64 = (Elf64_Sym *) data->d_buf;
                }

                // Print the symbol table
                printf("%-20s %s\n", "Name", "Value");
                for (int i = 0; i < symbols; ++i) {
                    if (sym32) {
                        if (sym32[i].st_shndx == SHN_UNDEF || sym32[i].st_shndx == SHN_COMMON ||
                            sym32[i].st_shndx == SHN_ABS) {
                            continue;
                        }
                        Elf_Scn *data_scn = elf_getscn(elf, sym32[i].st_shndx);
                        if (data_scn != NULL) {
                            Elf_Data *data_data = elf_getdata(data_scn, NULL);
                            if (data_data != NULL) {
                                if (GELF_ST_TYPE(sym32[i].st_info) == STT_OBJECT) {
                                    printf("%-20s %d\n", elf_strptr(elf, shdr.sh_link, sym32[i].st_name),
                                           (((int*)data_data->d_buf)[sym32[i].st_value/sizeof (int)]));
                                }
                            } else {
                                printf("%-20s %s\n", elf_strptr(elf, shdr.sh_link, sym32[i].st_name), "NULL");
                            }
                        }
                    } else if (sym64) {
                        if (sym64[i].st_shndx == SHN_UNDEF || sym64[i].st_shndx == SHN_COMMON ||
                            sym64[i].st_shndx == SHN_ABS) {
                            continue;
                        }
                        Elf_Scn *data_scn = elf_getscn(elf, sym64[i].st_shndx);
                        if (data_scn != NULL) {
                            Elf_Data *data_data = elf_getdata(data_scn, NULL);
                            if (data_data != NULL) {
                                if (GELF_ST_TYPE(sym64[i].st_info) == STT_OBJECT) {
                                    printf("%-20s %d\n", elf_strptr(elf, shdr.sh_link, sym64[i].st_name),
                                           (int)(((int*)data_data->d_buf)[sym64[i].st_value/sizeof (int)]));
                                }
                            } else {
                                printf("%-20s %s\n", elf_strptr(elf, shdr.sh_link, sym64[i].st_name), "NULL");
                            }
                        }
                    }
                }
            }
        }
    }
}

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
