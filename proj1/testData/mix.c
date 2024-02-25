#include <stdio.h>
int a = 10;
int b = 42;
int x;
int main() {
    int c = a + b;
    printf("%d\n", c);
    for (int i = 0; i < 10; i++) {
        printf("i=%d\n", i);
    }
    return 0;
}