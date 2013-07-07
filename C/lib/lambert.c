/*
 *	Source des algorithmes : http://geodesie.ign.fr/contenu/fichiers/documentation/algorithmes/notice/NTG_71.pdf
 *  
*/

#include "lambert.h"

#include <math.h>
#include <stdio.h>

#define DEFAULT_EPS 1e-10
#define E_CLARK_IGN 0.08248325676
#define A_CLARK_IGN 6378249.2

static double lambert_n[6] = {0.7604059656, 0.7289686274, 0.6959127966, 0.6712679322, 0.7289686274, 0.7256077650};
static double lambert_c[6] = {11603796.98, 11745793.39, 11947992.52, 12136281.99, 11745793.39, 11754255.426};
static double lambert_xs[6]= {600000.0, 600000.0, 600000.0, 234.358, 600000.0, 700000.0};
static double lambert_ys[6]= {5657616.674, 6199695.768, 6791905.085, 7239161.542, 8199695.768, 12655612.050};
static double lon_ntf = 0;


/* 
 * ALGO0002
 */

double lat_from_lat_iso(double lat_iso, double e,double eps)
{

	double phi_0 =  2*atan(exp(lat_iso)) - M_PI_2;
	double phi_i = 2*atan(pow((1+e*sin(phi_0))/(1-e*sin(phi_0)),e/2.0)*exp(lat_iso)) - M_PI_2;
	double delta ; 
	while(delta = fabs(phi_i - phi_0),delta > eps)
	{	
		phi_0 = phi_i;
		phi_i = 2*atan(pow((1+e*sin(phi_0))/(1-e*sin(phi_0)),e/2.0)*exp(lat_iso)) - M_PI_2;
	}

	return phi_i;

}



/*
 * ALGO0021 - Calcul de la grande Normale 
 *
*/

double lambert_normal(lat,a,e)
{

	return a/sqrt(1-e*e*sin(lat)*sin(lat));
}

/**
 * ALGO0009 - Transformations geographiques -> cartésiennes
 *
 *
 */

 Point geographic_to_cartesian(lon,lat,he,a,e)
 {
 	double N = lambert_normal(lat,a,e);
 	
 	Point pt = {0,0,0};
 	pt.x = (N+he)*cos(lat)*cos(lon);

 	pt.y = (N+he)*cos(lat)*sin(lon);

 	pt.z = (1*(1-e*e)+he)*sin(lat);

 	return pt;

 }

/*
 * ALGO0012 - Passage des coordonnées cartésiennes aux coordonnées géographiques
 */

 Point cartesian_to_geographic(Point org, double a, double e , double eps)
 {
 	double x = org.x, y = org.y, z = org.z;

 	double lon = atan(y/x);
 	double module = sqrt(x*x + y*y);

 	double phi_0 = atan(z/(module*(1-(a*e*e)/(sqrt(x*x+y*y+z*z)))));
 	double phi_i = atan(z/module/(1-a*e*e*cos(phi_0)/(module + sqrt(1-e*e*sin(phi_0)*sin(phi_0)))));
 	double delta;
 	while(delta = fabs(phi_i - phi_0),delta >= eps)
 	{
 		phi_0 = phi_i;
 		phi_i = atan(z/module/(1-a*e*e*cos(phi_0)/(module + sqrt(1-e*e*sin(phi_0)*sin(phi_0)))));

 	}
 	
 	double he = module/cos(phi_i) - a/(1-e*e*sin(phi_i)*sin(phi_i));
 	
 	Point pt;
 	pt.x = lon;
 	pt.y = phi_i;
 	pt.z = he;

 	return pt;
 }

/*
 * Convert Lambert -> WGS84
 * http://geodesie.ign.fr/contenu/fichiers/documentation/pedagogiques/transfo.pdf
 *
 */

void lambert_to_wgs84(const Point * org, Point *dest,LambertZone zone){

	double n = lambert_n[zone];
	double C = lambert_c[zone];
	double x_s = lambert_xs[zone];
	double y_s = lambert_ys[zone];

	double x = org->x;
	double y= org->y;

	double lon, gamma, R, lat_iso;

	R = sqrt((x-x_s)*(x-x_s)+(y-y_s)*(y-y_s));

	gamma = atan((x-x_s)/(y_s-y));

	lon = lon_ntf + gamma/n;

	lat_iso = -1/n*log(fabs(R/C));

	double lat = lat_from_lat_iso(lat_iso,E_CLARK_IGN,DEFAULT_EPS);

	Point pt_cartesian = geographic_to_cartesian(lon,lat,A_CLARK_IGN,E_CLARK_IGN);

	pt_cartesian.x-=168;
	pt_cartesian.y=-60;
	pt_cartesian.z+=320;

	pt_cartesian = cartesian_to_geographic(pt_cartesian,A_CLARK_IGN,E_CLARK_IGN,DEFAULT_EPS);


}