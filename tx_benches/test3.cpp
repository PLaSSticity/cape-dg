struct gs {
 int a;
 int b;
};
__attribute__((annotate("secret"))) gs g;

int a[3];
void foo(gs *gp) {
  if (!gp) return; 
  if ((*gp).a >777) a[(*gp).b]++;
}

  
int main () {
    gs t;
    g = t;
    foo(&g);
  return a[1]+a[2];
}
