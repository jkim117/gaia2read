#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>


#include "astromath.h"
#include "mmath.h"
#include "gaia2ret.h"
#include "astrometry.h"
#include "gaia2cat.h"
#include "gaiastar.h"
#include "gaia2idsort.h"
#include "mmath.h"
#include "utils.h"
#include "sllist.h"
#include "pmotion.h"

IDElement recurseID(long start, long end, long gaiaID, FILE *idFile);
gaiastar getStarfromID(long gaiaID, const double *epoch);

extern char catpath_Gaia2Bin[MAX_LINE];
extern char catpath_Gaia2Mass[MAX_LINE];

int starPosCount(double ra, double dec, bool circle, double frame_size,const double *epoch,
        const maglims* maglim)
{
  if (!circle)
    frame_size = frame_size/2;

  double ra_min = 0.0;
  double ra_max = 0.0;
  double dec_min = 0.0;
  double dec_max = 0.0;

  if ( frame_size > 0 ) {
    double pm_corr = 0;
    if ( epoch ) {
      double tdiff = abs(*epoch - 2015.5);
      // add another 0.1 mas/yr to PM to avoid any rounding errors
      pm_corr = 0.1 * ( 40000 + 1 ) * tdiff;
      pm_corr = MAS2DEG( pm_corr );
    }

    // calculate corner coordinates
    double search_size = frame_size+pm_corr;

    double ra_ll, dec_ll;     // lower left
    astr_rignomonic(
		    -search_size, -search_size, ra, dec, &ra_ll, &dec_ll
		    );

    double ra_ul, dec_ul;     // upper left
    astr_rignomonic(
		    -search_size, search_size, ra, dec, &ra_ul, &dec_ul
		    );

    double ra_lr, dec_lr;     // lower right
    astr_rignomonic(
		    search_size, -search_size, ra, dec, &ra_lr, &dec_lr
		    );

    double ra_ur, dec_ur;     // upper right
    astr_rignomonic(
		    search_size, search_size, ra, dec, &ra_ur, &dec_ur
		    );

    double ra_lm, dec_lm;     // lower middle
    astr_rignomonic(
		    0, -search_size, ra, dec, &ra_lm, &dec_lm
		    );

    double ra_um, dec_um;     // upper middle
    astr_rignomonic(
		    0, search_size, ra, dec, &ra_um, &dec_um
		    );

    // get min/max declinations
    dec_min = MIN( dec_ul, dec_um );
    dec_min = MIN( dec_min, dec_ll );
    dec_min = MIN( dec_min, dec_lm );

    dec_max = MAX( dec_ll, dec_lm );
    dec_max = MAX( dec_max, dec_um );
    dec_max = MAX( dec_max, dec_ul );

    // get min/max RA
    ra_min = MIN( ra_ul, ra_ll );
    ra_max = MAX( ra_ur, ra_lr );

    // check if pole is in the frame
    if ( dec - search_size <= -90.0 ) {
      dec_min = -90.0;
      ra_min = 0.0;
      ra_max = 360.0;
    }
    else if ( dec + search_size >= 90.0 ) {
      dec_max = 90.0;
      ra_min = 0.0;
      ra_max = 360.0;
    }
  }
  else {
    // full sky
    ra_min = 0;
    ra_max = 360;
    dec_min = -90;
    dec_max = 90;
  }

  if (circle)
    return posCount(ra_min,ra_max,dec_min,dec_max,test_starcirc, ra, dec, frame_size, epoch,
            maglim);
  else
    return posCount(ra_min,ra_max,dec_min,dec_max,test_star, ra, dec, frame_size, epoch,
            maglim);

}
// returns list of stars given size, circle or rectangular, and center ra and dec
int starPosSearch(double ra, double dec, bool circle, double frame_size, const double *epoch,
        const maglims* maglim, gaiastar* stars)
{
    if (!circle)
        frame_size = frame_size/2;

    double ra_min = 0.0;
    double ra_max = 0.0;
    double dec_min = 0.0;
    double dec_max = 0.0;

    if ( frame_size > 0 ) {
        double pm_corr = 0;
        if ( epoch ) {
	  double tdiff = abs(*epoch - 2015.5);
            // add another 0.1 mas/yr to PM to avoid any rounding errors
            pm_corr = 0.1 * ( 40000 + 1 ) * tdiff;
            pm_corr = MAS2DEG( pm_corr );
        }

        // calculate corner coordinates
        double search_size = frame_size+pm_corr;

        double ra_ll, dec_ll;     // lower left
        astr_rignomonic(
            -search_size, -search_size, ra, dec, &ra_ll, &dec_ll
        );

        double ra_ul, dec_ul;     // upper left
        astr_rignomonic(
            -search_size, search_size, ra, dec, &ra_ul, &dec_ul
        );

        double ra_lr, dec_lr;     // lower right
        astr_rignomonic(
            search_size, -search_size, ra, dec, &ra_lr, &dec_lr
        );

        double ra_ur, dec_ur;     // upper right
        astr_rignomonic(
            search_size, search_size, ra, dec, &ra_ur, &dec_ur
        );

        double ra_lm, dec_lm;     // lower middle
        astr_rignomonic(
            0, -search_size, ra, dec, &ra_lm, &dec_lm
        );

        double ra_um, dec_um;     // upper middle
        astr_rignomonic(
            0, search_size, ra, dec, &ra_um, &dec_um
        );

        // get min/max declinations
        dec_min = MIN( dec_ul, dec_um );
        dec_min = MIN( dec_min, dec_ll );
        dec_min = MIN( dec_min, dec_lm );

        dec_max = MAX( dec_ll, dec_lm );
        dec_max = MAX( dec_max, dec_um );
        dec_max = MAX( dec_max, dec_ul );

        // get min/max RA
        ra_min = MIN( ra_ul, ra_ll );
        ra_max = MAX( ra_ur, ra_lr );

        // check if pole is in the frame
        if ( dec - search_size <= -90.0 ) {
            dec_min = -90.0;
            ra_min = 0.0;
            ra_max = 360.0;
        }
        else if ( dec + search_size >= 90.0 ) {
            dec_max = 90.0;
            ra_min = 0.0;
            ra_max = 360.0;
        }
    }
    else {
        // full sky
        ra_min = 0;
        ra_max = 360;
        dec_min = -90;
        dec_max = 90;
    }

    if (circle)
      return posQuery(ra_min,ra_max,dec_min,dec_max,test_starcirc, ra, dec, frame_size, epoch,
              maglim, stars);
    else
      return posQuery(ra_min,ra_max,dec_min,dec_max,test_star, ra, dec, frame_size, epoch,
              maglim, stars);
}

typedef struct
{
  long gaia;
  long tmass;
  long hat;
}IDFull;

long findNewID(long start, long end, long ID, IDFull* idFile, IDType intype, IDType outtype);

sllist* starListToIDs(gaiastar stars[], IDType outID, int count)
{
    sllist *idList = NULL;
    if(count <= 0) return idList;

    char *gaiaFileName = concat(catpath_Gaia2Mass,"IDgaiaSort");
    int gaiaFileFd = open(gaiaFileName, O_RDONLY);
    if (gaiaFileFd < 0) {
        printf("ERROR: could not open file %s\n", gaiaFileName);
        exit(EXIT_FAILURE);
    }

    // Get size of file
    struct stat buf;
    fstat(gaiaFileFd, &buf);
    off_t size = buf.st_size;

    // Read file using mmap
    IDFull* gaiaFile = mmap(0, size, PROT_READ, MAP_SHARED, gaiaFileFd, 0);
    if (gaiaFile == MAP_FAILED) {
        printf("ERROR: errno %d\n", errno);
        printf("ERROR: could not read file %s\n", gaiaFileName);
        exit(EXIT_FAILURE);
    }
    long numIDs = size / sizeof(IDFull);

    for (int i = 0; i < count; i++) {
        const gaiastar* star = &stars[i];
        long gaiaID = star->source_id;
        long other = findNewID(0, numIDs, gaiaID, gaiaFile, GAIA, outID);
        slappend(&idList,&other,sizeof(long));
    }

    int ret = munmap(gaiaFile, size);
    if (ret < 0) {
        printf("ERROR: could not unmap memory\n");
        exit(EXIT_FAILURE);
    }
    close(gaiaFileFd);
    return idList;
}

// should work on all id types, including a gaia, in which it just returns otherIDt
char* toGaiaID(const char* otherID, IDType inID, char* buffer)
{
    if (inID==GAIA)
        return (char*)otherID;

    char *sortedFileName = NULL;
    if (inID==TMASS)
        sortedFileName = concat(catpath_Gaia2Mass,"IDtmassSort");
    else
        sortedFileName = concat(catpath_Gaia2Mass,"IDhatSort");
    int sortedFileFd = open(sortedFileName, O_RDONLY);
    if (sortedFileFd < 0) {
        printf("ERROR: could not open file %s\n", sortedFileName);
        exit(EXIT_FAILURE);
    }

    // Get size of file
    struct stat buf;
    fstat(sortedFileFd, &buf);
    off_t size = buf.st_size;

    // Read file using mmap
    IDFull* sortedFile = mmap(0, size, PROT_READ, MAP_SHARED, sortedFileFd, 0);
    if (sortedFile == MAP_FAILED) {
        printf("ERROR: could not read file %s\n", sortedFileName);
        exit(EXIT_FAILURE);
    }
    long numIDs = size / sizeof(IDFull);

    long other = strtol(otherID, NULL, 10);
    long gaiaID = findNewID(0, numIDs, other, sortedFile, inID, GAIA);

    int ret = munmap(sortedFile, size);
    if (ret < 0) {
        printf("ERROR: could not unmap memory\n");
        exit(EXIT_FAILURE);
    }
    close(sortedFileFd);

    sprintf(buffer,"%ld",gaiaID);
    return buffer;
}


// uses a binary search algorithm to find an ID within one of the sorted files
long findNewID(long start, long end, long ID, IDFull* idFile, IDType intype, IDType outtype)
{
    while(start <= end)
    {
        long mid = (end-start)/2+start;
        IDFull current = idFile[mid];

        long currentID;
        if (intype==GAIA)
            currentID = current.gaia;
        else if(intype==TMASS)
            currentID = current.tmass;
        else //HAT
            currentID = current.hat;

        long compare = ID - currentID;

        if (compare < 0)
            end = mid - 1;
        else if (compare > 0)
            start = mid + 1;
        else {
            if (outtype == GAIA)
                return current.gaia;
            else if (outtype == TMASS)
                return current.tmass;
            else
                return current.hat;
        }
    }
    return 0;
}



// get list of stars from a list of Gaia IDs
int starsfromID(sllist* longIDs, const double *epoch,gaiastar* stars)
{
  int i = 0;
	for (const sllist* it = longIDs; it != NULL; it = it->next)
	{
		char *ptr;
		long id = strtol(it->data, &ptr, 10);
		gaiastar nextStar = getStarfromID(id, epoch);
		stars[i]=nextStar;
		i++;
	}
	return 0;

}

// get one star from one id
gaiastar getStarfromID(long gaiaID, const double *epoch)
{
	char stringID[20];
	sprintf(stringID,"%ld",gaiaID);

    // based on the first char of the gaiaID, look in the appropriate id file
	char *fileName;
	int numStars;
	if (stringID[0]=='1')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id1");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id1";
	  numStars = 116724371;
	}
	else if (stringID[0]=='2')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id2");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id2";
	  numStars = 163285128;
	}
	else if (stringID[0]=='3')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id3");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id3";
	  numStars = 87098579;
	}
	else if (stringID[0]=='4')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id4");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id4";
	  numStars = 585793870;
	}
	else if (stringID[0]=='5')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id5");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id5";
	  numStars = 531853513;
	}
	else if (stringID[0]=='6')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id6");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id6";
	  numStars = 203648450;
	}
	else if (stringID[0]=='7')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id7");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id7";
	  numStars = 1990817;
	}
	else if (stringID[0]=='8')
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id8");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id8";
	  numStars = 3698075;
	}
	else
	{
	  fileName = concat(catpath_Gaia2Bin,"IDSTSort/id9");
	  //fileName = "/home/jkim/work/Gaia2Bin/IDSTSort/id9";
	  numStars = 6507262;
	}

	FILE *idFile = fopen(fileName,"rb");
    // use binary search to find the id within the file
	IDElement targetID = recurseID(0, numStars, gaiaID, idFile);
	fclose(idFile);
	free(fileName);
	if (targetID.sourceID==0)
	{
		printf("ERROR: Gaia ID Query does not exist");
		exit(EXIT_FAILURE);
	}
	char zone[4];
	sprintf(zone,"%d",targetID.zone);
	char * catpath = concat(catpath_Gaia2Bin,"sortedBin/z");
	char *zoneFileName = concat(catpath,zone);

	FILE *zoneFile = fopen(zoneFileName,"rb");
	fseek(zoneFile,targetID.position,SEEK_SET);
	gaiastar targetStar;
	fread((void*)&targetStar,sizeof(gaiastar),1,zoneFile);
	fclose(zoneFile);
	free(zoneFileName);
	free(catpath);

    // apply proper motion if necessary
    if ( epoch ) {
        const double tdiff = ( *epoch - 2015.5 );
        pmotion_apply( &targetStar.ra, &targetStar.dec,targetStar.pmra, targetStar.pmdec, tdiff );
    }
	return targetStar;
}

// uses a binary search algorithm to find the id within a file. If not found, it returns NULL
IDElement recurseID(long start, long end, long gaiaID, FILE *idFile)
{
	while(start <= end)
	{
		long mid = (end-start)/2+start;
		fseek(idFile,mid*sizeof(IDElement),SEEK_SET);
		long fileID;
		fread(&fileID,sizeof(long),1,idFile);

		char idStringInput[20];
		char idStringFile[20];
		sprintf(idStringInput,"%ld",gaiaID);
		sprintf(idStringFile,"%ld",fileID);

		int compare = strcmp(idStringInput,idStringFile);

		if (compare < 0)
		{
			end = mid-1;
		}
		else if(compare > 0)
		{
			start = mid+1;
		}
		else
		{
			fseek(idFile,mid*sizeof(IDElement),SEEK_SET);
			IDElement idTarget;
			fread((void*)&idTarget,sizeof(IDElement),1,idFile);
			return idTarget;
		}
	}
    IDElement null;
    null.sourceID = 0;
    null.position = 0;
    null.zone = 0;
	return null;

}

// checks to make sure star is within box and applies proper motion
bool test_star(gaiastar* star, double centRA, double centDec, double half_size, const double *epoch,
        const maglims* maglim)
{
  // check magnitude limits
  if ( maglim ) {
    if ( maglim->minGmag && star->phot_g_mean_mag < *maglim->minGmag)
        return false;
    if ( maglim->maxGmag && star->phot_g_mean_mag > *maglim->maxGmag)
        return false;
    // Not all Gaia stars have RP and BP magnitudes in GaiaDR2, so check for this
    // n/a values were stored as 3.55: see DataPreparation/gaia2writebin.c
    if (maglim -> minBpmag && (fabs(3.55 - star->phot_bp_mean_mag)<1e-7
            || star->phot_bp_mean_mag < *maglim->minBpmag))
        return false;
    if (maglim -> maxBpmag && (fabs(3.55 - star->phot_bp_mean_mag)<1e-7
            || star->phot_bp_mean_mag > *maglim->maxBpmag))
        return false;
    if (maglim -> minRpmag && (fabs(3.55 - star->phot_rp_mean_mag)<1e-7
            || star->phot_rp_mean_mag < *maglim->minRpmag))
        return false;
    if (maglim -> maxRpmag && (fabs(3.55 - star->phot_rp_mean_mag)<1e-7
            || star->phot_rp_mean_mag > *maglim->maxRpmag))
        return false;
  }

  // apply pm
  if ( epoch ) {
    const double tdiff = ( *epoch - 2015.5 );
      pmotion_apply( &star->ra, &star->dec,star->pmra, star->pmdec, tdiff );
  }

    if ( half_size > 0 ) {
        real xi, eta;
        astr_rgnomonic(
            star->ra, star->dec,
            centRA, centDec,
            &xi, &eta
        );

        if (    xi > half_size
            ||  xi < -half_size
            ||  eta > half_size
            ||  eta < -half_size
        ) {
            return false;
        }
    }

    return true;
}

// tests star to make sure it is within the circle and then applies proper motion
bool test_starcirc(gaiastar* star,double centRA,double centDec,double radius, const double *epoch,
        const maglims* maglim)
{
  // check magnitude limits
  if ( maglim ) {
    if ( maglim->minGmag && star->phot_g_mean_mag < *maglim->minGmag)
        return false;
    if ( maglim->maxGmag && star->phot_g_mean_mag > *maglim->maxGmag)
        return false;
    // Not all Gaia stars have RP and BP magnitudes in GaiaDR2, so check for this
    // n/a values were stored as 3.55: see DataPreparation/gaia2writebin.c
    if (maglim -> minBpmag && (fabs(3.55 - star->phot_bp_mean_mag)<1e-7
            || star->phot_bp_mean_mag < *maglim->minBpmag))
        return false;
    if (maglim -> maxBpmag && (fabs(3.55 - star->phot_bp_mean_mag)<1e-7
            || star->phot_bp_mean_mag > *maglim->maxBpmag))
        return false;
    if (maglim -> minRpmag && (fabs(3.55 - star->phot_rp_mean_mag)<1e-7
            || star->phot_rp_mean_mag < *maglim->minRpmag))
        return false;
    if (maglim -> maxRpmag && (fabs(3.55 - star->phot_rp_mean_mag)<1e-7
            || star->phot_rp_mean_mag > *maglim->maxRpmag))
        return false;
  }
  // apply pm
  if ( epoch ) {
    const double tdiff = ( *epoch - 2015.5 );
      pmotion_apply( &star->ra, &star->dec,star->pmra, star->pmdec, tdiff );
  }

    real xi, eta;
    astr_rgnomonic(
        star->ra, star->dec,
        centRA, centDec,
        &xi, &eta
    );

    if (    xi > radius
        ||  xi < -radius
        ||  eta > radius
        ||  eta < -radius
    ) {
        return false;
    }

    if ( mdist2( xi, eta ) > radius * radius ) {
        return false;
    }

    return true;
}




