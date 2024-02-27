#include <stdio.h>
char z = 5;
int a = 10;
static int b __attribute__((section(".my_data_section"))) = 42;
char y;
int x;
int main() {
    int c = a + b;
    printf("%d\n", c);
    for (int i = 0; i < 10; i++) {
        printf("i=%d\n", i);
    }
    return 0;
}