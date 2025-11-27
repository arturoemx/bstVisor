#include <simple2d.h>
#include <BST.h>
#include <iostream>
#include <vector>

using namespace std;

#define FONT "./GandhiSans-Regular.otf"

template < typename X > struct visData
{
   X data;
   float x, y;
   float theta, mag;
   int size;
   int nivel;
   S2D_Text *txt;
     visData ()
   {
      data = 0;
      x = y = 0;
      theta = mag = 0;
      size = 1;
      nivel = 0;
      txt = nullptr;
   }
   visData (const X & d)
   {
      data = d;
      x = y = 0;
      theta = mag = 0;
      size = 1;
      nivel = 0;
      txt = nullptr;
   }
   X operator () ()
   {
      return data;
   }
   bool operator < (visData < X > &v)
   {
      if (data < v.data)
         return true;
      return false;
   }
};

template < typename X > ostream & operator << (ostream & s, visData < X > &v)
{
   s << "[" << v.data << ", (" << v.x << ", " << v.y << "),"
      << " (" << v.mag << "," << v.mag << ") | nv= "
      << v.nivel << ", sz= " << v.size << "]";
   return s;
}

BST < visData < int >>*tree = nullptr, basura;
S2D_Window *window = nullptr;
vector < vector < nodoT < visData < int >>*>>Niveles;
float nodeRadius;

void on_key (S2D_Event e)
{
   // Check `e.key` for the key being interacted with

   switch (e.type)
   {
   case S2D_KEY_DOWN:
      cout << "key : " << e.key << endl;
      if (!strncmp (e.key, "Escape", 6))
         S2D_Close (window);

      // Key was pressed
      break;

   case S2D_KEY_HELD:
      // Key is being held down
      break;

   case S2D_KEY_UP:
      // Key was released
      break;
   }
}

void Update ()
{

}


void bstSizeInfo (nodoT < visData < int > >*r, int nv, int &nvCount,
                  int &treeSize)
{
   int localTreeSize = 0;
   if (!r)
      return;

   if (nv > nvCount)
      nvCount = nv;
   treeSize++;
   localTreeSize = treeSize;
   bstSizeInfo (r->izq, nv + 1, nvCount, treeSize);
   bstSizeInfo (r->der, nv + 1, nvCount, treeSize);
   r->dato.size = 1 + treeSize - localTreeSize;
   r->dato.nivel = nv;
}

void recorre (nodoT < visData < int > >*r, unsigned int nivel,
              vector < vector < nodoT < visData < int > >*> >&Nv)
{
   if (r == nullptr)
      return;

   while (nivel >= Nv.size ())
   {
      vector < nodoT < visData < int >>*>l;

      Nv.push_back (l);
   }
   Nv[nivel].push_back (r);
   recorre (r->izq, nivel + 1, Nv);
   recorre (r->der, nivel + 1, Nv);
}

void _calculaCoors (nodoT < visData < int > >*r, unsigned int nivel,
                    vector < vector < nodoT < visData < int > >*> >&Nv,
                    float cX, float cY, float R, float incR, float angle,
                    float magSlice)
{
   if (r == nullptr)
      return;

   float angleIzq, angleDer;

   angleIzq = angle + magSlice / 4;
   angleDer = angle - magSlice / 4;
   r->dato.x = cX + R * cosf (angle);
   r->dato.y = cY + R * sinf (angle);
   r->dato.mag = R;
   r->dato.theta = angle;
   r->dato.txt = S2D_CreateText (FONT, to_string (r->dato.data).c_str (), 20);
   if (r->dato.txt)
   {
      r->dato.txt->x = r->dato.x - 15;
      r->dato.txt->y = r->dato.y - 8;
      r->dato.txt->color.r = 0.5;
      r->dato.txt->color.g = 1.0;
      r->dato.txt->color.b = 0.0;
      r->dato.txt->color.a = 1.0;
   }
   _calculaCoors (r->izq, nivel + 1, Nv, cX, cY, R + incR, incR, angleIzq,
                  magSlice * 0.5);
   _calculaCoors (r->der, nivel + 1, Nv, cX, cY, R + incR, incR, angleDer,
                  magSlice * 0.5);
}

void calculaCoors (S2D_Window *win, BST < visData < int > >&T,
                   vector < vector < nodoT < visData < int > >*> >&Nv)
{
   float R, maxR, incR, cX, cY, angle;
   int ancho, alto, treeSize, nNiveles;

   recorre (T.raiz, 0, Nv);

   ancho = win->viewport.width;
   alto = win->viewport.height;
   cX = ancho / 2;
   cY = alto / 2;


   maxR = min (cX, cY);
   incR = maxR / Nv.size ();
   nodeRadius = incR / 3;
   R = 0;

   treeSize = 0;
   nNiveles = 0;
   bstSizeInfo (T.raiz, 0, nNiveles, treeSize);


   angle = 0.5 * M_PI;
   _calculaCoors (T.raiz, 0, Nv, cX, cY, R, incR, angle, 2 * M_PI);
}


void Render ()
{
   unsigned int i, j;
   nodoT < visData < int >>*nodo;
   float cX, cY;

   if (Niveles.size () <= 0)
      return;
   nodo = Niveles[0][0];
   cX = nodo->dato.x;
   cY = nodo->dato.y;

   for (i = Niveles.size () - 1; i > 0; --i)
   {
      if (Niveles[i].size () <= 0)
         continue;
      nodo = Niveles[i][0];
      S2D_DrawCircle (cX, cY, nodo->dato.mag, 90, 0.5, 0.5, 0.0, 0.6);
      S2D_DrawCircle (cX, cY, nodo->dato.mag - 2, 90, 0, 0, 0.0, 1);
   }

   for (i = 0; i < Niveles.size (); ++i)
      for (j = 0; j < Niveles[i].size (); ++j)
      {
         nodo = Niveles[i][j];

         if (nodo->izq != nullptr)
            S2D_DrawLine (nodo->dato.x, nodo->dato.y,
                          nodo->izq->dato.x, nodo->izq->dato.y, 3,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
         if (nodo->der != nullptr)
            S2D_DrawLine (nodo->dato.x, nodo->dato.y,
                          nodo->der->dato.x, nodo->der->dato.y, 3,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

         S2D_DrawCircle (nodo->dato.x, nodo->dato.y, nodeRadius, 72, 0, 0.9,
                         0.7, 1);
         S2D_DrawCircle (nodo->dato.x, nodo->dato.y, nodeRadius - 2, 72, 0,
                         0.0, 0.0, 1);
         if (nodo->dato.txt != nullptr)
            S2D_DrawText (nodo->dato.txt);

      }
}

int main (int argc, char **argv)
{
   int i, N = 16;
   long semilla = 0;
   int ancho = 1920, alto = 1080;

   if (argc > 1)
      N = atoi (argv[1]);
   if (argc > 2)
      semilla = atol (argv[2]);

   cout << endl
      << "Se inicializó el generador de número aleatorios con "
      << semilla << endl << endl;

   //Llenamos el árbol
   tree = new BST < visData < int >>(&basura);

   srand48 (semilla);

   for (i = 0; i < N; ++i)
   {
      visData < int >val ((int) (lrand48 () % 1000));

      tree->inserta (val);
      cout << "Insertamos el valor " << val << " al árbol." << endl;
   }
   cout << endl;



   window = S2D_CreateWindow ("bstVisor", ancho, alto, Update, Render, 0);
   window->on_key = on_key;

   calculaCoors (window, *tree, Niveles);

   cout << "tree =" << *tree << endl;

   cout << "Niveles[" << Niveles.size () << "]:" << endl;
   for (i = 0; i < (int) Niveles.size (); ++i)
   {
      int n, j;

      n = Niveles[i].size ();
      cout << "Niveles[" << i << "|" << n << "] = {";
      if (Niveles[i].size () > 0)
      {
         for (j = 0; j < n - 1; ++j)
            cout << (Niveles[i][j]->dato) << ", ";
         cout << (Niveles[i][j]->dato);
      }
      cout << "}" << endl;
   }
   cout << endl;



   S2D_Show (window);

   cout << "Bye, bye" << endl;


   //Liberamos Memoria.
   basura.borraArbol ();
   if (tree != nullptr)
      delete tree;


   return 0;
}
