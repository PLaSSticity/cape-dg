
__attribute__((annotate("secret"))) int g;

int a[3];

  
int main () {
  g = 333;
  int* gp = &g;
  if (!gp) {
    return a[1];
  }

  if (*gp == 333)
    return a[2];

  // *gp = *gp + 1;
  return a[0];
}
