//
//  Framework for a raytracer
//  File: sphere.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "sphere.h"
#include <iostream>
#include <math.h>
#include <algorithm>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // place holder for actual intersection calculation
	double a = ray.D.dot(ray.D);
	double b = 2*(ray.D.dot(ray.O - position));
	double c = (((ray.O.x - position.x) * (ray.O.x - position.x)) + ((ray.O.y - position.y) * (ray.O.y - position.y)) + ((ray.O.z - position.z) * (ray.O.z - position.z))- (r * r));
	double d = ((b*b) - (4 * a*c));
	if (d<0) {
		return Hit::NO_HIT();
	}
	double rootsPlus = ((-b + sqrt(d)) / (2 * a));
	double rootsMinus = ((-b - sqrt(d)) / (2 * a));
	double t = 0.0;
	if (rootsPlus > 0 && rootsMinus < 0)
	{
		t = rootsPlus;
	}
	else if (rootsMinus > 0 && rootsPlus < 0)
	{
		t = rootsMinus;
	}
	else if (rootsMinus < 0 && rootsPlus < 0)
	{
		return Hit::NO_HIT();
	}
	else
	{
		t = min(rootsPlus, rootsMinus);
	}

    //Vector OC = (position - ray.O).normalized();
	//double t = min(rootsPlus, rootsMinus);
	//double tt = max(rootsPlus, rootsMinus);

	//idea... seems to work
	/*t = max(0.0, t);
	if (t == 0)
	{
		return Hit::NO_HIT();
	}*/

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Vector N = ((ray.O + (ray.D * t)) - position).normalized();

    return Hit(t,N);
}
