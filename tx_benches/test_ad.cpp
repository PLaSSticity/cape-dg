__attribute__((annotate("secret"))) int g;

  int a[3];
int main() {
  a[g] ++;
  return 0;
}
