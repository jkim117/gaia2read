
#ifndef PROPER_MOTION_H__
#define PROPER_MOTION_H__

#include "astrometry.h"


/* ========================================================================= */
/* calculates proper motion mu_ra*cos Dec, mu_dec [mas/year] */
void    pmotion(
            const skypos*   pos1,       /* RA, Dec [deg] */
            const skypos*   pos2,       /* RA, Dec [deg] */
            real            tdiff,      /* time difference [Julian Years] */
            real*           pm_ra,      /* mu_ra*cos Dec [mas/yr] */
            real*           pm_dec      /* mu_dec [mas/yr] */
        );

/* calculates total proper motion and position angle */
void    pmotion_total(
            const skypos*   pos1,       /* RA, Dec [deg] */
            const skypos*   pos2,       /* RA, Dec [deg] */
            real            tdiff,      /* time difference [Julian Years] */
            real*           pm_tot,     /* total proper motion [mas/yr] */
            real*           pos_angle   /* position angle [deg] */
        );

/* calculates total proper motion and position angle from proper motion */
void    pmotion2total(
            real            pm_ra,      /* mu_ra*cos Dec [mas/yr] */
            real            pm_dec,     /* mu_dec [mas/yr] */
            real*           pm_tot,     /* total proper motion [mas/yr] */
            real*           pm_angle    /* position angle [deg] */
        );

/* apply true angular proper motion correction to sky coordinates */
void    pmotion_apply(
            double*         ra,        /* [deg] in/out */
            double*         dec,       /* [deg] in/out */
            real            pm_ra,      /* angular proper motion [mas/yr] */
            real            pm_dec,     /* mu_dec [mas/yr] */
            real            tdiff       /* time difference [yr] */
        );

/* apply coordinate proper motion correction to sky coordinates */
void    pmotion_apply_coordpm(
            double*         ra,        /* [deg] in/out */
            double*         dec,
            real            pm_ra,      /* coordinate proper motion [ms/yr] */
            real            pm_dec,     /* mu_dec [mas/yr] */
            real            tdiff       /* time difference [yr] */
        );

/* apply precession and proper motion correction to sky position */
void    pmotion_precess(
            skypos*         pos,        /* [deg] in/out */
            real            JD0,        /* starting epoch [Julian days] */
            real            JD,         /* final epoch */
            real            pm_ra,      /* [mas/yr] or [ms/yr] */
            real            pm_dec,     /* mu_dec [mas/yr] */
            bool            use_ang_pm  /* use [mas/yr] (true) or [ms/yr] */
        );

/* apply precession and proper motion correction (starting epoch: J2000.0 */
void    pmotion_precess2000(
            double*         ra,        /* [deg] in/out */
            double*         dec,
            real            JD,         /* final epoch [Julian days] */
            real            pm_ra,      /* mu_ra*cos Dec [mas/yr] */
            real            pm_dec,     /* mu_dec [mas/yr] */
            bool            use_ang_pm  /* use [mas/yr] (true) or [ms/yr] */
        );

/* ========================================================================= */
#endif // PROPER_MOTION_H__
