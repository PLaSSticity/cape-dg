/* memcpy example */
#include <string.h>
__attribute__((annotate("secret"))) int g = 999;

int a[3];
void foo(int *gp) {
  if (!gp) return; 
  if ((*gp) >777) a[(*gp)]++;
}

  
int main () {
    g = 777;
    foo(&g);
  return a[1]+a[2];
}
