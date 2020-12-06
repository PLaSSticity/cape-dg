
__attribute__((annotate("secret"))) int g;

int a[3];
void foo(int gg) {
  // if (!gp) return; 
  if (gg >0) a[gg]++;
}

void bar(int gg) {
  foo(gg);
}

int main () {
    foo(g);
    if (g > 0) {
      bar(g);  
    }
  return a[1]+a[2];
}
