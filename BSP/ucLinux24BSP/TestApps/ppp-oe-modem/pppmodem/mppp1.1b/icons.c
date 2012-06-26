#include "xhead.h"
#include "xmppp.h"
#include <X11/xpm.h>

#include "pppon.xpm"
#include "pppoff.xpm"

#include "mppp.h"
/*
 *  CreatePPPONPixmap()   - create the on pixmap for xmppp icon
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Pixmap pixmap   on success
 *      NUL             on failure
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      need Xpm library
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

Pixmap CreatePPPONPixmap(void)
{
    int
        rc;

    XpmAttributes
        xpmatts;

    Pixmap
        mask,
        pix;

    mask=(Pixmap) NULL;
    xpmatts.closeness=65536;
    xpmatts.valuemask = XpmSize | XpmCloseness;

    rc=XpmCreatePixmapFromData(GetDisplay(),GetrootWindow(),
        pppon_xpm,
        &pix,
        &mask,
        &xpmatts);

    if (mask != (Pixmap) NULL)
        XFreePixmap(GetDisplay(),mask);

    if (rc != XpmSuccess)
        return ((Pixmap) NULL);

    return(pix);
}

/*
 *  CreatePPPONFFixmap()   - create the off pixmap for xmppp icon
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Pixmap pixmap   on success
 *      NUL             on failure
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      need Xpm Library
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

Pixmap CreatePPPOFFPixmap(void)
{
    int
        rc;

    XpmAttributes
        xpmatts;

    Pixmap
        mask,
        pix;

    mask=(Pixmap) NULL;
    xpmatts.closeness=65536;
    xpmatts.valuemask = XpmSize | XpmCloseness;

    rc=XpmCreatePixmapFromData(GetDisplay(),GetrootWindow(),
        pppoff_xpm,
        &pix,
        &mask,
        &xpmatts);

    if (mask != (Pixmap) NULL)
        XFreePixmap(GetDisplay(),mask);

    if (rc != XpmSuccess)
        return ((Pixmap) NULL);

    return(pix);
}
