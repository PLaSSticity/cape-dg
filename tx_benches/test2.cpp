
__attribute__((annotate("secret"))) int* g;

void foo(int* a) {
while (a[1] <0) 
    a[1]++;
}

  
int main () {

int gg = 0;
g = &gg;

int a[3] = {-3, -3, -3};
  if (*g > 0) {
    foo(a);
  } else {
  }
  return a[1]+a[2];
}
