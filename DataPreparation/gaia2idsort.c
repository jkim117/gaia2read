#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gaia2idsort.h"

int idcmp(const void * a, const void * b)
{
  IDElement* id1 = (IDElement*)a;
  IDElement* id2 = (IDElement*)b;
  char idString1[20];
  char idString2[20];
  sprintf(idString1,"%ld",id1->sourceID);
  sprintf(idString2,"%ld",id2->sourceID);
  return strcmp(idString1,idString2);
}

int writeBin(char *outName, IDElement idArray[], int numStars)
{
  FILE *outFile = fopen(outName,"wb");
  fwrite(idArray,sizeof(IDElement),numStars,outFile);
  fclose(outFile);
  return 0;
}

int main(void)
{
  // for each file in the IDST folder
  for(int i =1; i < 10; i++)
    {
      char *fileName;
      int numStars;
      if (i==1)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id1";
	  numStars = 116724371;
	}
      if (i==2)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id2";
	  numStars = 163285128;
	}
      if (i==3)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id3";
	  numStars = 87098579;
	}
      if (i==4)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id4";
	  numStars = 585793870;
	}
      if (i==5)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id5";
	  numStars = 531853513;
	}
      if (i==6)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id6";
	  numStars = 203648450;
	}
      if (i==7)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id7";
	  numStars = 1990817;
	}
      if (i==8)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id8";
	  numStars = 3698075;
	}
      if (i==9)
	{
	  fileName = "/home/jkim/work/Gaia2Bin/IDST/id9";
	  numStars = 6507262;
	}

      // create and sort the IDElement array from the file
      IDElement *idArray = malloc(numStars*sizeof(IDElement));
      FILE *idFile = fopen(fileName,"rb");
      printf("%s\n",fileName);
      for (int j = 0; j < numStars; j++)
	{
	  long id;
	  long pos;
	  int zNumber;
	  fread((void*)(&id),sizeof(long),1,idFile);
	  fread((void*)(&pos),sizeof(long),1,idFile);
	  fread((void*)(&zNumber),sizeof(int),1,idFile);
	  IDElement newID = {id,pos,zNumber};
	  idArray[j] = newID;
	}
      fclose(idFile);

      //now sort here
      qsort(idArray,numStars,sizeof(IDElement),idcmp);

      //now write back the sorted array into the sorted IDST
      char *outFile;
      if (i==1)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id1";
      if (i==2)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id2";
      if (i==3)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id3";
      if (i==4)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id4";
      if (i==5)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id5";
      if (i==6)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id6";
      if (i==7)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id7";
      if (i==8)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id8";
      if (i==9)
	outFile = "/home/jkim/work/Gaia2Bin/IDSTSort/id9";

      writeBin(outFile, idArray, numStars);
      free(idArray);
    }

  return 0;
}
