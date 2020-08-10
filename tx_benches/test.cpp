
__attribute__((annotate("secret"))) int g;

int a[3];
void foo() {
if (a[1] >0) 
    a[1]++;
}

void boo() {
if (a[2] >0)
  a[2]++;
}
  
int main () {
  g = 0;
  if (g > 0) {
    foo();
  } else {
    boo();
  }
  return a[1]+a[2];
}
