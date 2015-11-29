#include <stdlib.h>
#include "tab.h"
#include "../klib.h"

void ordinamint(char* bid, unsigned int row, unsigned int col)
{
  for(int z= row; z > 0; z--)
    for(int x= 1; x < z; x++)
      if(bid[(x-1)*col] > bid[x*col]) // array[a*col+b] => array[a][b]
        scambia( &bid[(x-1)*col], &bid[x*col]);
}

void impli(char* minterm, char* impl, unsigned int* mintermini, unsigned int* implicanti, unsigned int len)
{
  int new= 0; //numero di minterm ridotti
  char m='0'; // valore corrente di match
  char (*temp)[len]= malloc(2048);
  int x= 0;

  while(x < *mintermini) // per ogni minterm
    {
      while(minterm[x*len]==m) //se ha valore m
        {
          for(int v= x+1; v < *mintermini; v++) // lo confronto con ogni minterm
            if(minterm[v*len]==m+1) // che abbia valore m+1
              if( compara( &minterm[x*len], &minterm[v*len]) == len-3) // se differiscono di uno oltre al primo
                {
                  compose( &minterm[x*len+1], &minterm[v*len+1], &temp[new][1]); // li riduco in temp
                  temp[new][0]= minterm[x*len]; // il numero di 1 è uguale
                  for(int r= 0; r < new; r++) // per ogni implicante
                    if( compara( &(temp[new])[1], &(temp[r])[1]) == len-2) // se è uguale ad un'altro
                      {
                        new--; // quindi verrà sovrascritto
                        break;
                      }
                  new++; // passo al prossimo implicante
                }
          ++x; // registro l'avanzamento sui minterm totali
        }
      ++m; // passo al prossimo gruppo di minterm con valore +1
    }

  for(int x= 0; x < *mintermini; x++) // per ogni minterm
    {
      int n= 0;
      for(int t= 0; t < new; t++) // per ogni implicante
        if( compare( &minterm[x*len+1], &(temp[t])[1]) == len-2) //se il minterm è coperto
          ++n;
      if(!n) // se un minterm non è coperto da nessun implicante
        {
          copia( &minterm[x*len+1], &impl[(*implicanti)*(len-1)] ); // lo salvo come implicante primo
          *implicanti+=1;
        }
    }
  *mintermini= new;
  over(temp[0], minterm, new*len); // sovrascrivo i minterm con i loro implicanti
  free(*temp); // libero la locazione di temp
  return ;
}

void compose(char* uno, char* due, char* out)
{
  int x= 0;
  while(x < strl(uno))
    {
      if(uno[x]==due[x])
        out[x]=uno[x];
      else
        out[x]='-'; // sostituisce i caratteri differenti con '-'
      ++x;
    }
  out[x]='\0';
}

unsigned int compare(char* uno, char* due)
{
  unsigned int match= 0;
  int x= 0;
  while(x < strl(uno))
    {
      if(uno[x]==due[x]||due[x]=='-')
        match++;
      ++x;
    }
  return match;
}

void over(char* uno, char* due, unsigned int tot)
{
  for(int x= 0; x <= tot; x++)
    due[x]= uno[x];
}

int essential(long* min, unsigned int* mintermini, char* impl , unsigned int* implicanti, char* tab , unsigned int len, char* ess, unsigned int *essen)
{
  char (*temp)[len]= malloc(1024); // array temporaneo per implicanti
  unsigned int minterms= 0; // mintermini non ancora coperti alla fine della funzione
  unsigned int imp= 0; // implicanti non ancora esaminati
  int ret= 0;

  for(int x= 0; x < *mintermini; x++) //per ogni minterm iniziale
    {
      min[minterms]= min[x]; //riporta il minterm
      for(int y= 0; y < *implicanti; y++) // per ogni implicante primo
        if(tab[x+y*(*mintermini)]=='x') // se trova una x (e qundi il minterm è coperto dall'implicante)
          {
            int n= 0;
            for(int z= y+1; z < *implicanti; z++) //per ogni implicante successivo
              if(tab[x+z**mintermini]=='x') // se il minterm è coperto nuovamente
                n++;
            if(n) // se viene coperto da altri implicanti
              minterms++;
            else
              {
                n= 0;
                for(int r=0; r < *essen; r++)
                  if( compara( &impl[y*len], &ess[r*len])== len-1)
                    n++;
                if(!n)
                  {
                    copia( &impl[y*len], &ess[*essen*len]);
                    *essen += 1;
                    ret= 1;
                  }
              }
            break;
          }
    }
  for(int x= 0; x < *implicanti; x++)
    {
      int n=0;
      for(int y= 0; y < *essen; y++)
        if( compara( &impl[x*len], &ess[y*len])== len-1)
          n++;
      if(!n)
        {
          copia( &impl[x*len], &impl[imp*len]);
          imp++;
        }
    }
  *implicanti= imp;
  *mintermini= minterms;
  free(*temp);
  return ret;
}
