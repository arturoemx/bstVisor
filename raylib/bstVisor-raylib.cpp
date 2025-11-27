#include <raylib.h>
#include <BST.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;


template < typename X > struct visualData
{
   X val;
   float x, y;
   float theta, mag;
   int nivel;
   int size;

   int nodeRadius;

   visualData ()
   {
      val = 0;
      x = y = 0;
      theta = mag = 0;
      size = 1;
      nivel = 0;
   }

   visualData (const X & d)
   {
      val = d;
      x = y = 0;
      theta = mag = 0;
      nivel = 0;
      size = 1;
   }
   X operator () ()
   {
      return val;
   }
   bool operator < (visualData < X > &v)
   {
      if (val < v.val)
         return true;
      return false;
   }
};

template < typename X > ostream & operator << (ostream & s, visualData < X > &v)
{
   s << "[" << v.val << ", (" << v.x << ", " << v.y << "),"
      << " (" << v.mag << "," << v.theta << ") | nv= "
      << v.nivel << ", sz= " << v.size << "]";
   return s;
}

template <typename Y>
struct visor: public BST< visualData< Y > >
{
   int ancho, alto;
   unsigned int nNiveles;
   int nodeRadius;
   float cX, cY;
   int iteracion;
   float Error;
   vector < vector < nodoT < visualData < Y >>*>>Niveles;
   bool changes;

   visor():BST< visualData<Y> >()
   {
      ancho = 800;
      alto = 600;
      nNiveles = 0;
      nodeRadius = 30;
      iteracion = 0;
      Error = 0;
      cX = ancho / 2;
      cY = alto / 2;
      changes = true;
   }

   visor (int _ancho, int _alto, BST< visualData<Y> > *T = nullptr):BST< visualData<Y> >(T)
   {
      ancho = _ancho;
      alto = _alto;
      nNiveles = 0;
      nodeRadius = 30;
      iteracion = 0;
      Error = 0;
      cX = ancho / 2;
      cY = alto / 2;
      changes = true;
   }

   void computeNodesInfo (nodoT < visualData < Y > >*r, int nv, int &nvCount,
                        int &treeSize)
   {
      int localTreeSize = 0;
      if (!r)
         return;

      if (nv > nvCount)
         nvCount = nv;
      treeSize++;
      localTreeSize = treeSize;
      computeNodesInfo (r->izq, nv + 1, nvCount, treeSize);
      computeNodesInfo (r->der, nv + 1, nvCount, treeSize);
      r->dato.size = 1 + treeSize - localTreeSize;
      r->dato.nivel = nv;
   }

   void _llenaListaNiveles (nodoT < visualData < Y > > *r)
   {
      if (r == nullptr)
         return;

      Niveles[r->dato.nivel].push_back (r);
      _llenaListaNiveles (r->izq);
      _llenaListaNiveles (r->der);
   }

   void llenaListaNiveles (nodoT < visualData < Y > >*r, int nNv)
   {
      unsigned int i;
      vector < nodoT < visualData < Y >>*>nNT;

      for (i = 0; i < Niveles.size (); ++i)
         Niveles[i].clear ();
      Niveles.clear ();

      Niveles = vector < vector < nodoT < visualData < Y >>*>>(nNv, nNT);

      _llenaListaNiveles (this->raiz);
   }

   void computeBSTInfo()
   {
      int treeSize, nNv;

      treeSize = 0;
      nNv = 0;

      computeNodesInfo (this->raiz, 0, nNv, treeSize);
      nNiveles = nNv + 1;
      llenaListaNiveles (this->raiz, nNiveles);
   }

   void _computeNodesCoors (nodoT < visualData < Y > >*r, unsigned int nivel,
                       float R, float incR, float angle, float magSlice)
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
      _computeNodesCoors (r->izq, nivel + 1, R + incR, incR, angleIzq, magSlice * 0.5);
      _computeNodesCoors (r->der, nivel + 1, R + incR, incR, angleDer, magSlice * 0.5);
   }

   void computeNodesCoors (float angle0 = M_PI)
   {
      float R, maxR, incR;
      
      maxR = min (cX, cY);
      incR = maxR / nNiveles;
      nodeRadius = incR / 3;
      R = 0;

      angle0 = 0.5 * M_PI;
      _computeNodesCoors (this->raiz, 0, R, incR, angle0, 2 * M_PI);
   }

   void computeAngles(nodoT < visualData < Y > > *r, float &thetaIzq, float &thetaDer)
   {
      float maxIzq, maxDer;

      maxIzq = thetaIzq;
      maxDer = thetaDer;
      thetaIzq = thetaDer = 0;    
      if (r->izq == nullptr && r->der != nullptr)
         thetaDer = maxIzq;
      else if (r->der == nullptr && r->izq != nullptr)
         thetaIzq = maxDer;
      else
      {
         float propI, propD, total;
         total = r->dato.size - 1;
         propI = (float)r->izq->dato.size / total;
         propD = (float)r->der->dato.size / total;
         thetaIzq = maxIzq * propI;
         thetaDer = maxDer * propD;
      }
   }

   float _updateNodesCoors(nodoT < visualData < Y > > *r, float incR, float idealAngle, float etaMag, float etaTheta)
   {
      if (r == nullptr)
         return 0;

      /*
        ErrorMag = (incR * nivel - mag)^2
                 = incrR^2 * nivel^2 - 2*incR*nivel*mag+mag^2
        dErrorMag/dmag = -2*incR*nivel+2*mag;
        ErrorThetaIzq = ((padre.theta-idealAngle/2)-theta)^2
                     =   padre.theta^2-2*padre.theta.idealAngle/2+idealAngle^2/4 -2*(padre.theta-idealAngle/2)*theta+theta^2)
        dErrorThetaIzq/dThetaIzq =
                         -2*(padre.theta-idealAngle/2)+2*theta
        ErrorThetaDer = ((padre.theta-idealAngle/2)+theta)^2
                     =   padre.theta^2+2*padre.theta.idealAngle/2+idealAngle^2/4 -2*(padre.theta+idealAngle/2)*theta+theta^2)
        dErrorThetaDer/dThetaDer =
                         -2*(padre.theta+idealAngle/2)+2*theta
      */
      
      float Emag, Etheta, dEmag, dEtheta, thetaIzq, thetaDer, propTheta;
      
      
      propTheta = idealAngle / r->padre->dato.theta;

      Emag = pow(incR * r->dato.nivel - r->dato.mag, 2);

      dEmag = -2 * (incR * r->dato.nivel - r->dato.mag);

      if (r->padre->izq == r) //El nodo está a la izquierda
      {
         Etheta = pow((r->padre->dato.theta - propTheta * idealAngle)-r->dato.theta, 2);
         dEtheta = -2 * (r->padre->dato.theta - propTheta * idealAngle - r->dato.theta);
         cout << "Objetivo:" << incR * r->dato.nivel << ", " << (r->padre->dato.theta - 0.5*idealAngle) * 180 / M_PI << endl;
      }
      else
      {
         Etheta = pow((r->padre->dato.theta + propTheta * idealAngle)-r->dato.theta, 2);
         dEtheta = -2 * (r->padre->dato.theta + propTheta * idealAngle - r->dato.theta);
         cout << "Objetivo:" << incR * r->dato.nivel << ", " << (r->padre->dato.theta + 0.5*idealAngle) * 180 / M_PI << endl;
      }

      cout << "[dEmag, dEtheta | Emag, Etheta] = [" << dEmag << ", " << dEtheta * 180 / M_PI 
           << "|" << Emag << ", " << Etheta * 180 / M_PI<< "]" << endl << endl;

      r->dato.mag += -etaMag * dEmag;
      r->dato.theta += -etaTheta * dEtheta;
      
      thetaIzq = thetaDer = idealAngle;
      computeAngles(this->raiz, thetaIzq, thetaDer);
      cout << "Nivel " << r->dato.nivel << ": {" << thetaIzq * 180 / M_PI << ", " << thetaDer * 180 / M_PI<<  "}" << endl;
      _updateNodesCoors(r->izq, incR, thetaIzq, etaMag, etaTheta);
      _updateNodesCoors(r->der, incR, thetaDer, etaMag, etaTheta);
      r->dato.x = cX + r->dato.mag * cosf(r->dato.theta);
      r->dato.y = cY + r->dato.mag * sinf(r->dato.theta);
      return Emag + Etheta;
   }

   float updateNodesCoors(float etaMag=0.1, float etaTheta=0.1)
   {
      float maxR, incR, thetaIzq, thetaDer;
      maxR = 2 * min (cX, cY) / 3;
      incR = (maxR - 30) / nNiveles;
      nodeRadius = incR / 3;

      if (this->raiz == nullptr)
         return 0;
      this->raiz->dato.theta = 2 * M_PI;
      this->raiz->dato.mag   = 0;
      this->raiz->dato.x = cX;
      this->raiz->dato.y = cY;

      if (this->raiz->dato.size == 1)
         return 0;

      thetaIzq = thetaDer = M_PI * 2;
      computeAngles(this->raiz, thetaIzq, thetaDer);
      cout << "Nivel 0: {" << thetaIzq * 180 / M_PI << ", " << thetaDer * 180 / M_PI << "}" << endl;
      return _updateNodesCoors(this->raiz->izq, incR,  thetaIzq, etaMag, etaTheta) + 
             _updateNodesCoors(this->raiz->der, incR,  thetaDer, etaMag, etaTheta);
   }

   void drawTree()
   {
      unsigned int i, j;
      nodoT < visualData < int >>*nodo;
      Vector2 center = {cX, cY};
      float fontFactor, fontSize;
      int xTxt, yTxt;

      fontFactor = nodeRadius / 30.f;
      fontSize = fontFactor * 25;
            
      for (i = Niveles.size () - 1; i > 0; --i)
      {
         if (Niveles[i].size () <= 0)
            continue;
         nodo = Niveles[i][0];
         DrawRing(center, nodo->dato.mag, nodo->dato.mag+5, 0.f, 360.f, (int)0, Fade(MAROON, 0.15f));
      }

      for (i = 0; i < Niveles.size (); ++i)
         for (j = 0; j < Niveles[i].size (); ++j)
         {
            nodo = Niveles[i][j];
            Vector2 fin;
            string txt = to_string(nodo->dato.val);

            
            center = {nodo->dato.x, nodo->dato.y};   
            if (nodo->izq != nullptr)
            {
               fin = {nodo->izq->dato.x, nodo->izq->dato.y};
               DrawLineEx(center, fin, 3, RED);
            }   
            if (nodo->der != nullptr)
            {
               fin = {nodo->der->dato.x, nodo->der->dato.y};
               DrawLineEx(center, fin, 3, RED);
            }   
            
            DrawCircleV(center, nodeRadius, Fade(SKYBLUE, 0.5f + 0.5f));
            DrawRing(center, nodeRadius, nodeRadius+3, 0.f, 360.f, (int)0, Fade(DARKBLUE, 1.f));

            xTxt = (int) (center.x - fontSize * txt.length() / 4);
            yTxt = (int) (center.y - fontSize / 2);
            DrawText(txt.c_str(), xTxt, yTxt, fontSize, WHITE);
         }
   }

   void update ()
   {
      if (iteracion % 1 == 0)
      {
      
         if (changes == true)
         {
            computeBSTInfo();
            Error = updateNodesCoors();

            for (int i = 0; i < (int) Niveles.size (); ++i)
            {
               int n, j;
               n = Niveles[i].size ();
               if (n > 0)
               {
                  for (j = 0; j < n - 1; ++j)
                     cout << Niveles[i][j]->dato.mag  << ", ";
                  cout << Niveles[i][j]->dato.mag  << endl;
               }
               if (Error < 0.001)
                  changes = false;
            }
         }
         if (changes)
            iteracion++;
      }
   }

   void render ()
   {
      string iterText, errorText, spaceText;

      BeginDrawing ();

      if (changes)
      {
         ClearBackground (RAYWHITE);

         drawTree();

         iterText = to_string(iteracion);
         errorText = to_string(Error);
         spaceText = string(" | ");
         DrawText((iterText+spaceText+errorText).c_str(), 20, alto - 30, 20, BLACK);
      }

      EndDrawing ();
   }

   void loop ()
   {
      InitWindow (ancho, alto, "BST VISOR");
      SetTargetFPS (60);

      // Main loop
      iteracion = 0;
      while (!WindowShouldClose ()) // Detect window close button or ESC key
      {
         //Update
         update ();

         //Draw;
         render ();
         
      }
   }

   ~visor ()
   {
      CloseWindow ();
   }
};


int main (int argc, char **argv)
{
   int i, N = 16;
   long semilla = 0;
   int ancho = 1280, alto = 1024;
   BST<visualData<int>> Basura;
   visor<int> V(ancho, alto, &Basura);
   
   if (argc > 1)
      N = atoi (argv[1]);
   if (argc > 2)
      semilla = atol (argv[2]);

   cout << endl
      << "Se inicializó el generador de número aleatorios con "
      << semilla << endl << endl;

   //Llenamos el árbol
   srand48 (semilla);

   

   for (i = 0; i < N; ++i)
   {
      visualData < int >val ((int) (lrand48 () % 1000));

      V.inserta (val);
      cout << "Insertamos el valor " << val << " al árbol." << endl;
   }
   cout << endl;

   V.loop ();

   return 0;
}
