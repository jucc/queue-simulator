#include <cstdlib>
#include <cstdio>

using namespace std;

int main(int argc, char *argv[])
{
        int *numeros = (int *) malloc(100 * sizeof(int));
        int i;
        
        for (i = 0; i < 100; i++) {
                numeros[i] = i;                
        }
        
        for (i = 0; i < 100; i++) {
                printf ("%d\n", numeros[i]);
        }
}
