#include <BST.h>
#include <iostream>

using namespace std;

int main (int argc, char **argv)
{
   BST < long >basura;
   BST < long >A (&basura);
   int i, N = 16;
   long val, semilla = 0;
   nodoT < long >*apuMax, *apuMin;

   if (argc > 1)
      N = atoi (argv[1]);
   if (argc > 2)
      semilla = atol (argv[2]);

   cout << endl
      << "Se inicializó el generador de número aleatorios con "
      << semilla << endl << endl;
   srand48 (semilla);

   for (i = 0; i < N; ++i)
   {
      val = lrand48 () % 1000;
      A.inserta (val);
      cout << "Insertamos el valor " << val << " al árbol." << endl;
   }
   cout << endl;

   cout << "Este es el recorrido INORDER:" << endl;
   A.inorder ();
   cout << endl << endl;

   cout << "Este es el recorrido PREORDER:" << endl;
   A.preorder ();
   cout << endl << endl;

   cout << "Este es el recorrido POSORDER:" << endl;
   A.posorder ();
   cout << endl << endl;

   cout << "Encontramos los valores mínimos y máximos del árbol usando" <<
      endl << "métodos iterativos y recursivos." << endl << endl;

   apuMin = A.min ();
   apuMax = A.max ();
   cout <<
      "Los valores mínimos y máximos encontrados en forma iterativa son: "
      << apuMin->dato << " y " << apuMax->dato << endl << endl;

   apuMin = A.minR ();
   apuMax = A.maxR ();
   cout <<
      "Los valores mínimos y máximos encontrados en forma recursiva son: "
      << apuMin->dato << " y " << apuMax->dato << endl << endl;

   return 0;
}
