
__attribute__((annotate("secret"))) int g;

int a[3];
void foo(int gg) {
  // if (!gp) return; 
  if (gg >0) a[gg]++;
}

  
int main () {
    foo(g);
  return a[1]+a[2];
}
