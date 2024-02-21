#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <gelf.h>
#include <libelf.h>



int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        exit(1);
    }

    // Open the ELF file
    int fd = open(argv[1], O_RDONLY, 0);\
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // Initialize the ELF library
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed\n");
        exit(1);
    }

    // Open the ELF file
    Elf *elf = elf_begin((int) fd, ELF_C_READ, NULL);
    if (elf == NULL) {
        fprintf(stderr, "elf_begin() failed\n");
        exit(1);
    }

    // Get the ELF header
    GElf_Ehdr ehdr;
    if (gelf_getehdr(elf, &ehdr) == NULL) {
        fprintf(stderr, "gelf_getehdr() failed\n");
        exit(1);
    }

    // Print the ELF header
    printf("ELF Header:\n");
    printf("  Magic: ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", ehdr.e_ident[i]);
    }
    printf("\n");
    printf("  Class: %d-bit\n", ehdr.e_ident[EI_CLASS] == ELFCLASS32 ? 32 : 64);
    printf("  Data: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
    printf("  Version: %d\n", ehdr.e_ident[EI_VERSION]);
    printf("  OS/ABI: %d\n", ehdr.e_ident[EI_OSABI]);
    printf("  ABI Version: %d\n", ehdr.e_ident[EI_ABIVERSION]);
    printf("  Type: %d\n", ehdr.e_type);
    printf("  Machine: %d\n", ehdr.e_machine);
    printf("  Version: %d\n", ehdr.e_version);
    printf("  Entry point address: 0x%lx\n", ehdr.e_entry);
    printf("  Start of program headers: %ld (bytes into file)\n", ehdr.e_phoff);
    printf("  Start of section headers: %ld (bytes into file)\n", ehdr.e_shoff);
    printf("  Flags: 0x%x\n", ehdr.e_flags);
    printf("  Size of this header: %d (bytes)\n", ehdr.e_ehsize);
    printf("  Size of program headers: %d (bytes)\n", ehdr.e_phentsize);
    printf("  Number of program headers: %d\n", ehdr.e_phnum);
    printf("  Size of section headers: %d (bytes)\n", ehdr.e_shentsize);
    printf("  Number of section headers: %d\n", ehdr.e_shnum);
    printf("  Section header string table index: %d\n", ehdr.e_shstrndx);

    // Close the ELF file
    elf_end(elf);
    close(fd);

    return 0;
}