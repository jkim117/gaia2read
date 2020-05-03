#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * strsep(char **stringp, const char *delim);

typedef struct
{
  long tmass;
  long hat;

}ID2;

int idcmp(const void * a, const void * b)
{
  ID2* id1 = (ID2*)a;
  ID2* id2 = (ID2*)b;
  long result =  id1->tmass - id2->tmass;
  if (result<0)
    return -1;
  if (result>0)
    return 1;
  return 0;
}

// string concatenation
/*char *concat(const char *s1, const char *s2)
{
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1);
  if (result == NULL)
    {
      printf("Error: malloc failed in concat\n");
      exit(EXIT_FAILURE);
    }
  strcpy(result, s1);
  strcat(result, s2);
  return result;
  }*/

int main(void)
{
  char* catpath = "/home/jkim/catalogs/H-CAT/2MASS/2MASS_JH_AP/data/hat2masskeys/";
  long starCount = 0;
  ID2 *idArray = malloc(470992970*sizeof(ID2));
  //ID2 *idArray = malloc(153143*sizeof(ID2));
  int index = 0;
  for (int i = 1; i < 839; i++)
    {
      //      if( i!=15)
      //	continue;
      char* fileName;
      char* buffer;
      char temp[4];
      if (i < 10)
	{
	  sprintf(temp,"%d",i);
	  buffer = concat("00",temp);
	  fileName = concat(concat(catpath,buffer),".txt");
	}
      else if(i < 100)
	{
	  sprintf(temp,"%d",i);
	  buffer = concat("0",temp);
	  fileName = concat(concat(catpath,buffer),".txt");
	}
      else
	{
	  sprintf(buffer,"%d",i);
	  fileName = concat(concat(catpath,buffer),".txt");
	}
      FILE *file = fopen(fileName,"r");
      if ( file == NULL )
        {
	  printf("error: could not open file\n");
        }
      printf("%s\n",fileName);

      char line[20];
      long lineCount = 1;
        while(fgets(line, 20, file) != NULL)
	  {
	    //	    printf("%s\n",line);
	    //	  starCount++;
	    char firstHalf[9];
	    memcpy(firstHalf,line,8);
	    char secondHalf[8];
	    memcpy(secondHalf,&line[9],7);
	    
	    char* tmassString;
	    if (line[8]=='+')
	      {
		tmassString = concat(concat("1",firstHalf),secondHalf);
	      }
	    else
	      {
		tmassString = concat(concat("2",firstHalf),secondHalf);
	      }
	    char hatFirstHalf[8];
	    sprintf(hatFirstHalf,"%ld",lineCount);
	    char * hatString = concat(hatFirstHalf,buffer);
	    lineCount++;

	    long tmass;
	    long hatID;
	    
	    tmass=strtol(tmassString,NULL,10);
	    hatID=strtol(hatString,NULL,10);


	    //      	  printf("%ld %ld\n",tmass,hatID);
	    ID2 nextID;
	    nextID.tmass=tmass;
	    nextID.hat=hatID;
	  idArray[index] = nextID;
	  //	  printf("%ld %ld\n",idArray[index].tmass,idArray[index].hat);
      	  index++;
	  starCount++;
	  }
    }
  printf("%ld",starCount);
  printf("sorting...\n");
  qsort(idArray,470992970,sizeof(ID2),idcmp);
  //  qsort(idArray,153143,sizeof(ID2),idcmp);
  /*for (int i = 0; i < 153143; i++)
   {
     printf("%ld %ld\n",idArray[i].tmass,idArray[i].hat);
   }*/
  printf("writing...\n");
  FILE *outFile = fopen("/home/jkim/work/Gaia2Mass/hat2mass","wb");
  fwrite(idArray,sizeof(ID2),470992970,outFile);
  fclose(outFile);
  printf("done\n");
  return 0;
}
