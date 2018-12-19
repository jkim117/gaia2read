/******************************************************************************
 *  Name:    Jason Kim
 *  Program : Princeton USRP
 *
 *  Version: 1.1.2
 * 
 *  Description: Includes main method. Takes command line arguments, forwards queries to other programs, and writes output
 *
 ******************************************************************************/
#define _GNU_SOURCE
#include "utils.h"
#include "myargs.h"
#include "gaia2ret.h"
#include "astrio.h"
#include "sllist.h"
#include "astrometry.h"
#include "gaiaPrint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//*********************cmd-line arguments*********************
//options, ra and dec position with size and circular or rectangular plus
//
//includes option flag to show gaia id, hat id, 2mass id
//includes option flag to request based on input of gaia id, hat id, or 2 mass id

enum {
    arg_catpath,
    arg_header,
    //arg_outphot,
    //arg_estphot,
    arg_extra,
    arg_idrequest,
    arg_idtype,
    arg_idfile,
    arg_precess,
    arg_pm,
    arg_xieta,
    arg_rmin,
    arg_rmax,
    arg_bmin,
    arg_bmax,
    arg_cmdline
};

// array of longoptions (see myargs.c)
myoptlong longoptions[] =
{
    { "ra",             required_argument,  'r'         },
    { "dec",            required_argument,  'd'         },
    { "pos",            required_argument,  'p'         },
    { "size",           required_argument,  's'         },
    { "circ",           no_argument,        'c'         },
    { "id",             required_argument,  'g'         },
    { "idtype",         required_argument,   arg_idtype },
    { "cat",            required_argument,  arg_catpath },
    { "catalog",        required_argument,  arg_catpath },
    { "header",         no_argument,        arg_header  },
    { "extra",          no_argument,        arg_extra   },
    { "idrequest",     required_argument,  arg_idrequest  },
    { "idfile",         required_argument,  arg_idfile  },
    { "precess",        required_argument,  arg_precess },
    { "pm",             optional_argument,  arg_pm      },
    { "xieta-coords",   optional_argument,  arg_xieta   },
    { "mG",             optional_argument,  'm'         },
    { "MG",             optional_argument,  'M'         },
    { "mR",             optional_argument,  arg_rmin    },
    { "MR",             optional_argument,  arg_rmax    },
    { "mB",             optional_argument,  arg_bmin    },
    { "MB",             optional_argument,  arg_bmax    },
    { "cmdline",        no_argument,        arg_cmdline },
    { "out",            required_argument,  'o'         },
    { "version",        no_argument,        'v'         },
    { "help",           no_argument,        'h'         },
    { 0, 0, 0 }
};

// local functions
static int      add_star_to_list( sllist** ids, const char* id, bool isfile, IDType inputIDType);
static void     usage();
static void     help();
static void     version();

int main(int argc, char** argv)
{
	progname = mybasename( *argv ); // see utils.c
	//*********************Initialize cmd-line arguments*********************
	skypos center;
    bool cent_ra_set        = false;
    bool cent_dec_set       = false;
    double size             = 0;
    bool is_circular        = false;
    const char* outfile     = NULL;
    sllist* ids             = NULL;
    bool print_header       = false;
    bool print_extra        = false;
    IDType inputIDType      = GAIA;
    IDType specify_idOut    = GAIA;
    bool equinox            = false;
    double JDequinox        = 0;
    bool epoch              = false;
    double JD               = 0;
    skypos* xieta_center    = NULL;
    bool print_xieta        = false;
    bool print_cmdline      = false;
    const char* gID         = NULL;
    const char* idFile      = NULL;
    int idcount = 0;//number of id stars added to list
    int opt;
    bool set_maglim         = false;
    maglims maglim          = {
        NULL, NULL, NULL, NULL, NULL, NULL
    };


    if(argc == 1) {
      usage();
      exit(0);
    }

    while ((opt = mygetopt(argc, argv, "r:d:p:s:cg:o:m:M:vh", longoptions)) != NO_MORE_OPTIONS)
    {
        switch(opt)
        {
	        case 'r':           // --ra
	            if ( !astrio_parseRA( myoptarg, &center.RA, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid RA %s",
	                    progname, myoptarg
	                );
	            }

	            cent_ra_set = true;
	            break;

	        case 'd':           // --dec
	            if ( !astrio_parseDec( myoptarg, &center.Dec, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid Dec %s",
	                    progname, myoptarg
	                );
	            }

	            cent_dec_set = true;
	            break;

	        case 'p':           // --pos
	            if ( !astrio_parsepos( myoptarg, &center, NULL ) ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid position %s",
	                    progname, myoptarg
	                );
	            }

	            cent_ra_set = true;
	            cent_dec_set = true;
	            break;

	        case 's':           // --size
	            if ( !mystr2d( myoptarg, &size )  || size < 0.0 || size > 360.0 ) {
	                err_ret(
	                    EXIT_FAILURE, "%s: invalid argument %s",
	                    progname, myoptarg
	                );
	            }
	            break;

	        case 'c':           // --circ
	            is_circular = true;
	            break;

	        case 'o':           // --out
	            outfile = myoptarg;
	            break;

	        case 'g':           // --id
                gID = myoptarg;
	            break;

            case arg_catpath:   // --cat, --catalog
                gaia2_setpath( myoptarg );
                break;

	        case arg_idtype:    // --what type of ids are the input
                if ( !astrio_parseID(myoptarg, &inputIDType, NULL))
                {
                    err_ret(EXIT_FAILURE, "%s: invalid ID type %s", progname, myoptarg);
                }
	        	break;

	        case arg_header:    // --header
	            print_header = true;
	            break;

            case arg_extra:    // --extra info on star
                print_extra = true;
                break;

	        case arg_idrequest:     // --hat-id
                if ( !astrio_parseID(myoptarg, &specify_idOut, NULL))
                {
                    err_ret(EXIT_FAILURE, "%s: invalid ID type %s", progname, myoptarg);
                }
	            break;

	        case arg_idfile:    // --idfile
                idFile = myoptarg;
	            break;

	        case arg_precess:   // --precess
	            {
	                if ( !astrio_text2jd( myoptarg, &JDequinox, NULL ) ) {
	                    err_print_msg( "invalid date-time %s", myoptarg );
	                    usage();
	                }//JDequinox is in years

	                equinox = true;
	            }
	            break;

	        case arg_pm:        // --pm
	            {
	                if ( !myoptarg || !astrio_text2jd( myoptarg, &JD, NULL ) ) {
	                    err_print_msg("error with arg_pm", myoptarg);
	                }//JD is in years
	                else {
	                    epoch = true;
	                }
	            }
	            break;

            case arg_xieta:     // --xieta-coords
                {
                    if ( !myoptarg && cent_ra_set && cent_dec_set ) {
                        // Use search center for xi-eta coordinates
                        xieta_center = &center;
                    }
                    else if ( myoptarg ) {
                        char* myoptargcopy = strdup(myoptarg);
                        // Read in coordinates for frame-projection
                        char* pch = strtok(myoptargcopy, ",");
                        double xieta_ra = strtof(pch, NULL);
                        pch = strtok(NULL, ",");
                        if (pch == NULL) {
                            err_print_msg("Need to provide both RA and DEC for --xieta-coords");
                        }
                        else {
                            double xieta_dec = strtof(pch, NULL);
                            skypos xietapos = { xieta_ra, xieta_dec };
                            xieta_center = &xietapos;
                            print_xieta = true;
                        }
                        free(myoptargcopy);
                    }
                    else {
                        err_print_msg("Incorrect arguments provided to --xieta-coords");
                    }
                }
                break;

            case 'm':           // --mG
                {
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Gmin = strtof(myoptarg, NULL);
                        if (Gmin <= 0.0)
                            err_print_msg("invalid G mag bright cutoff", myoptarg);
                        else {
                            maglim.minGmag = &Gmin;
                            set_maglim = true;
                        }
                    }
                }
                break;

            case 'M':           // --MG
                {
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Gmax = strtof(myoptarg, NULL);
                        if (Gmax <= 0.0)
                            err_print_msg("invalid G mag faint cutoff", myoptarg);
                        else {
                            maglim.maxGmag = &Gmax;
                            set_maglim = true;
                        }
                    }
                }
                break;

            case arg_rmin:           // --mR
                {
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Rmin = strtof(myoptarg, NULL);
                        if (Rmin <= 0.0)
                            err_print_msg("invalid RP mag bright cutoff", myoptarg);
                        else {
                            maglim.minRpmag = &Rmin;
                            set_maglim = true;
                        }
                    }
                }
                break;

            case arg_rmax:           // --MR
                {
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Rmax = strtof(myoptarg, NULL);
                        if (Rmax <= 0.0)
                            err_print_msg("invalid RP mag faint cutoff", myoptarg);
                        else {
                            maglim.maxRpmag = &Rmax;
                            set_maglim = true;
                        }
                    }
                }
                break;

            case arg_bmin:           // --mB
                {
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Bmin = strtof(myoptarg, NULL);
                        if (Bmin <= 0.0)
                            err_print_msg("invalid BP mag bright cutoff", myoptarg);
                        else {
                            maglim.minBpmag = &Bmin;
                            set_maglim = true;
                        }
                    }
                }
                break;

            case arg_bmax:           // --MB
                {
                    printf("BP mag max = %.2f\n", strtof(myoptarg, NULL));
                    if ( !myoptarg ) {
                        err_print_msg("error with magnitude", myoptarg);
                    }
                    else {
                        double Bmax = strtof(myoptarg, NULL);
                        if (Bmax <= 0.0)
                            err_print_msg("invalid BP mag faint cutoff", myoptarg);
                        else {
                            maglim.maxBpmag = &Bmax;
                            set_maglim = true;
                        }
                    }
                }
                break;

	        case arg_cmdline:   // --cmdline
	            print_cmdline = true;
	            break;

	        case 'h':           // --help
	            help();

	        case 'v':           // --version
	            version();

	        default:
	            usage();
        }
    }

    /* Make sure the catalog path is set */
    gaia2_getpath( );

    // collect ID information from arguments 'g' and arg_idfile
    if (gID != NULL)
        idcount = add_star_to_list( &ids, gID, false, inputIDType );
    if (idFile != NULL)
        idcount = add_star_to_list( &ids, idFile, true, inputIDType );

    // sanity check on cmd-line arguments
    if (    ( cent_ra_set && !cent_dec_set )
        ||  ( !cent_ra_set && cent_dec_set )
    ) {
        err_print_msg( "invalid input coordinate" );
        usage();
    }

    if ( cent_ra_set && size <= 0.0 ) {
        err_print_msg( "invalid or missing frame size" );
        usage();
    }


	//*********************Collect Input from Command Line*********************
    if ( myoptind == argc ) {
        // no more command line arguments
        if ( ids == NULL ) {
            if (!cent_ra_set) {
                if ( size > 0 ) {
                    err_print_msg( "nothing to search for" );
                    usage();
                }

                // full sky search
                cent_ra_set = true;
                is_circular = false;
                size = -1;
            }
        }
        else {
            if ( cent_ra_set ) {
                err_print_msg(
                    "cannot do area and ID search at the same time"
                );
                usage();
            }
        }
    }
    else {
        if ( cent_ra_set ) {
            err_print_msg(
                "cannot do area and ID search at the same time"
            );
            usage();
        }
		//This here takes the remaining command line arguments, if there are any, as star ids and then adds them to the lsit
        // add IDs to list
        for ( ; myoptind < argc; ++myoptind ) {
            const char* id = argv[myoptind];   // shortcut
            add_star_to_list( &ids, id, false, inputIDType );
        }
    }


    //*********************Initializing stars list*********************
    FILE* os = NULL;
    const double* pJD = epoch ? &JD : NULL;
    const maglims* pmaglim = set_maglim ? &maglim : NULL;

    if ( cent_ra_set ) {
      int count;
      if ( !is_circular ) {
          // read square count
          count = starPosCount(center.RA, center.Dec, false, size, pJD, pmaglim);
      }
      else {
          // read circle count
          count = starPosCount(center.RA, center.Dec, true, size, pJD, pmaglim);
      }
      gaiastar *stars;
      stars=malloc(count*sizeof(gaiastar));

        if ( !is_circular ) {
            // read square
            starPosSearch(center.RA, center.Dec, false, size, pJD, pmaglim, stars);
        }
        else {
            // read circle
            starPosSearch(center.RA, center.Dec, true, size, pJD, pmaglim, stars);
        }

        if (count==0 ) {
            err_print_msg( "no star found" );
            exit( EXIT_FAILURE );
        }

        if ( outfile ) {
            os = fopen( outfile, "w" );
            if ( !os ) {
                err_ret( 11, "%s: cannot open file %s", progname, outfile );
            }
        }

        if ( equinox )
            gaia2_precesslist( stars, JDequinox,count );

        if ( !os )
            os = stdout;

        if ( print_header )
            gaiastar_printheader( os, print_extra, specify_idOut, print_xieta);

        if ( print_cmdline ) {
            fputs( "# ", os );
            myargs_print_cmdline( os, argc, argv );
        }

        if (specify_idOut==GAIA)
            gaiastar_printlist(os, stars, count, print_extra, xieta_center);
        else {
            sllist* altIDs = starListToIDs(stars,specify_idOut,count);
            gaiastar_printlist_alternateID(os, stars, count, print_extra, altIDs, specify_idOut, xieta_center);
        }

    }
    else {
        // get stars based on their IDs
        gaiastar *stars;
        stars = malloc(idcount*sizeof(gaiastar));
	    starsfromID(ids, pJD,stars);

	    if (idcount==0) {
            err_print_msg( "no star found" );
            exit( EXIT_FAILURE );
        }

        if ( outfile ) {
            os = fopen( outfile, "w" );
            if ( !os ) {
                err_ret( 11, "%s: cannot open file %s", progname, outfile );
            }
        }

        if ( equinox )
            gaia2_precesslist( stars, JDequinox ,idcount);

        if ( !os )
            os = stdout;

        if ( print_header )
            gaiastar_printheader( os, print_extra, specify_idOut, print_xieta);

        if ( print_cmdline ) {
            fputs( "# ", os );
            myargs_print_cmdline( os, argc, argv );
        }

        if ( specify_idOut==GAIA )
            gaiastar_printlist(os, stars, idcount, print_extra, xieta_center);
        else {
            sllist* altIDs = starListToIDs(stars,specify_idOut,idcount);
            gaiastar_printlist_alternateID(os, stars, idcount, print_extra, altIDs, specify_idOut, xieta_center);
        }
    }

    if ( outfile )
        fclose( os );

	exit(EXIT_SUCCESS);
}

// method to change formatted input HAT or 2MASS ID to a long form
local int toLongID(const char* id,IDType inputIDType,char* longID)
{
  if(inputIDType==HAT)
    {
      unsigned int i1;
      unsigned int i2;
      unsigned int i;
      i = 0;
      while(!(id[i] >= '0' && id[i] <= '9')) i++;
      sscanf(&(id[i]),"%u-%u",&i1,&i2);
      sprintf(longID,"%u%u",i2,i1);
      //longID[strlen(id)-4]=id[0];
      //longID[strlen(id)-3]=id[1];
      //longID[strlen(id)-2]=id[2];
      //unsigned int i;
      //for(i = 4; i < strlen(id);i++)
      //{
      //  longID[i-4]=id[i];
      //}
      //longID[i-1]='\0';
      //printf("%s %s\n",id,longID);
      return 0;

    }
  else if(inputIDType==TMASS)
    {
      if(id[8]=='+')
	longID[0]='1';
      else
	longID[0]='2';

      longID[1]=id[0];
      longID[2]=id[1];
      longID[3]=id[2];
      longID[4]=id[3];
      longID[5]=id[4];
      longID[6]=id[5];
      longID[7]=id[6];
      longID[8]=id[7];
      longID[9]=id[9];
      longID[10]=id[10];
      longID[11]=id[11];
      longID[12]=id[12];
      longID[13]=id[13];
      longID[14]=id[14];
      longID[15]=id[15];
      longID[16]='\0';
      return 0;
    }

  return 0;
}

int  add_star_to_list( sllist** ids, const char* id, bool isfile, IDType inputIDType)
{
  int idcount=0;
    if ( !id || !*id ) {
        return idcount;
    }

    if ( isfile || *id == '@' ) {
        const char* filename = !isfile ? &id[1] : id;
        FILE* idfile = fopen( filename, "r" );
        if ( !idfile ) {
            err_print_msg( "cannot open input ID file %s", filename );
            return idcount;
        }

        char* line = malloc( MAX_LINE * sizeof *line );
        size_t len = 0;
        while ( getline( &line, &len, idfile ) > 0 ) {
            char* comment = strchr( line, '#' );
            if ( comment ) {
                *comment = '\0';
            }
	    char idBuffer[30];
	    if(inputIDType==GAIA)
	      {
		char* idstr = toGaiaID(( char* )empty_string( line ),inputIDType,idBuffer);
		if ( idstr && *( idstr = trim_right( idstr ) ) ) {
		  slappend( ids, idstr, strlen( idstr ) + 1 );
		  idcount++;
		}
	      }
	    else
	      {
		char longid[20];
		//            char* idstr = ( char* )empty_string( line );
		toLongID(( char* )empty_string( line ),inputIDType,longid);
		char* idstr = toGaiaID(longid,inputIDType,idBuffer);
		if ( idstr && *( idstr = trim_right( idstr ) ) ) {
		  slappend( ids, idstr, strlen( idstr ) + 1 );
		  idcount++;
		}
	      }
	}

        free( line );
        fclose( idfile );
    }
    else {
      char idBuffer[30];
      if(inputIDType==GAIA)
	{
	  char *idstr = toGaiaID(id,inputIDType,idBuffer);
	  slappend( ids, idstr, strlen( idstr ) + 1 );
	}
      else
	{
	  char longid[20];

	  toLongID(id,inputIDType,longid);

	  char *idstr = toGaiaID(longid,inputIDType,idBuffer);
	  slappend( ids, idstr, strlen( idstr ) + 1 );
	}
      

	idcount++;
    }
    return idcount;
 }

void usage()
{
    static const char* helpstr[] =
{
"",
"Usage:\n",
"  gaia2read [options] <ID...>",
" or",
"  gaia2read [options] --size|-s <size> --pos|-p <center>",
" or",
"  gaia2read [options] [--pm [<epoch>]] --fits|-f <FITS>",
"",
" Options:",
" --ra|-r <ra>          : RA of the field center: [deg] or [HH:MM:SS.SSS]",
" --dec|-d <dec>        : Dec of the field center: [deg] or [DD:MM:SS.SSS]",
" --pos|-p <pos>        : coordinates of the field center (same as -r and -d)",
" --size|-s <size>      : [deg] length of the box or radius of circle",
" --circ|-c             : circular field instead of box",
" --id|-g <ID>          : retrieve source <ID> from catalogue",
" --idtype              : GAIA, HAT, TMASS, the type of ID that the input is given in",
//" --cat <path>          : top-level catalog path",
" --header              : print header",
" --extra               : print extra values including phot information and luminosity/radius",
" --idrequest           : GAIA, HAT, TMASS, the type of ID that the output gives",
" --idfile <path>       : read IDs (see option -g) from file",
" --precess <equinox>   : apply correction for precession for a given equinox",
" --pm <epoch>          : apply correction for proper motions. Epoch is in years",
" --xieta-coords <ra>,<dec> : output xi/eta coordinates centered at given sky position, defaulting to search center",
" --mG|-m <G_min>       : bright G magnitude cutoff",
" --MG|-M <G_max>       : faint G magnitude cutoff",
" --mf <f_min>          : minimum magnitude in band f, where f is 'G', 'R', or 'B'",
" --Mf <f_max>          : maximum magnitude in band f, where f is 'G', 'R', or 'B'",
" --out|-o <file>       : output file",
" --cmdline             : prints command line first",
" --version|-v          : prints out version",
" --help|-h             : print help screen",
NULL
};

    display_longtext( stderr, helpstr );
    exit( EXIT_FAILURE );
}

void help()
{
    static const char* helpstr[] =
{
"",
"Usage:\n",
"  gaia2read [options] <ID...>",
" or",
"  gaia2read [options] --size|-s <size> --pos|-p <center>",
" or",
"  gaia2read [options] [--pm [<epoch>]] --fits|-f <FITS>",
"",
" Options:",
" --ra|-r <ra>          : RA of the field center: [deg] or [HH:MM:SS.SSS]",
" --dec|-d <dec>        : Dec of the field center: [deg] or [DD:MM:SS.SSS]",
" --pos|-p <pos>        : coordinates of the field center (same as -r and -d)",
" --size|-s <size>      : [deg] length of the box or radius of circle",
" --circ|-c             : circular field instead of box",
" --id|-g <ID>          : retrieve source <ID> from catalogue",
" --idtype              : GAIA, HAT, TMASS, the type of ID that the input is given in",
//" --cat <path>          : top-level catalog path",
" --header              : print header",
" --extra               : print extra values including phot information and luminosity/radius",
" --idrequest           : GAIA, HAT, TMASS, the type of ID that the output gives",
" --idfile <path>       : read IDs (see option -g) from file",
" --precess <equinox>   : apply correction for precession for a given equinox",
" --pm <epoch>          : apply correction for proper motions. Epoch is in years",
" --out|-o <file>       : output file",
" --cmdline             : prints command line first",
" --version|-v          : prints out version",
" --help|-h             : print help screen",
NULL
};

    display_longtext( stdout, helpstr );
    exit( EXIT_SUCCESS );
}

void version()
{
    static const char* vString[] = 
    {
        "",
        "Name:    Jason Kim",
        "Program : Princeton USRP",
        "",
        "Version: 1.1.2",
        "",
        "Description: Includes main method. Takes command line arguments, forwards queries to other programs, and writes output",
        NULL
    };
    display_longtext( stdout, vString );
    exit( EXIT_SUCCESS );
}

