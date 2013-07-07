#include "basic.h"

#define DEFAULT_EPS 1e-10
#define E_CLARK_IGN 0.08248325676
#define A_CLARK_IGN 6378249.2
#define LON_MERI_NTF 0
#define LON_MERID_GREENWICH 0.04079234433
#define E_WGS84 0.08181919106

typedef enum {
	LAMBERT_I=0,
	LAMBERT_II=1,
	LAMBERT_III=2,
	LAMBERT_IV=3,
	LAMBERT_II_E=4,
	LAMBER_93= 5
} LambertZone;


/*
 * Convert a Point struct from one lambert zone to WGS84 
 *
 */
void lambert_to_wgs84(const Point * org, Point *dest, LambertZone zone);

/*
 * ALGO0002
 */

double lat_from_lat_iso(double lat_iso, double e, double eps);

/*
* ALGO0012
*/

Point cartesian_to_geographic(Point org, double a, double e , double eps);


void lambert_to_geographic(const Point * org,Point *dest, LambertZone zone, double lon_merid, double e, double eps);