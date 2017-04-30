//
//  Framework for a raytracer
//  File: scene.cpp
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

#include "scene.h"
#include "material.h"
#include <math.h>
#include <algorithm>

Color Scene::trace(const Ray &ray, int reflectIteration, int refractIteration, bool inside)
{
	// Find hit object and distance
	Hit min_hit(std::numeric_limits<double>::infinity(), Vector());
	Object *obj = NULL;
	for (unsigned int i = 0; i < objects.size(); ++i) {
		Hit hit(objects[i]->intersect(ray));
		if (hit.t < min_hit.t) {
			min_hit = hit;
			obj = objects[i];
		}
	}

	// No hit? Return background color.
	if (!obj) return Color(0.0, 0.0, 0.0);

	Material *material = obj->material;            //the hit objects material
	Point hit = ray.at(min_hit.t);                 //the hit point
	Vector N = min_hit.N;                          //the normal at hit point
	Vector V = -ray.D;     //the view vector


	/****************************************************
	* This is where you should insert the color
	* calculation (Phong model).
	*
	* Given: material, hit, N, V, lights[]
	* Sought: color
	*
	* Hints: (see triple.h)
	*        Triple.dot(Vector) dot product
	*        Vector+Vector      vector sum
	*        Vector-Vector      vector difference
	*        Point-Point        yields vector
	*        Vector.normalize() normalizes vector, returns length
	*        double*Color        scales each color component (r,g,b)
	*        Color*Color        dito
	*        pow(a,b)           a to the power of b
	****************************************************/

	Color color, ambient, diffuse, specular, reflective, refractive;
	//set all the lights to 0 to start
	ambient.set(0.0, 0.0, 0.0);
	diffuse.set(0.0, 0.0, 0.0);
	specular.set(0.0, 0.0, 0.0);
	reflective.set(0.0, 0.0, 0.0);
	refractive.set(0.0, 0.0, 0.0);

	//test each point for each light
	for (int j = 0; j < lights.size(); j++)
	{
		//a unit vector in the direction towards the light
		Vector L = (lights[j]->position - hit).normalized();
		Vector R = (2 * (L.dot(N)*N) - L);

		//A ray from the hit point towards the light
		Ray shadowTest(hit + L, L);
		//a vector from the hit point to the light, to get the length
		//Vector testStuff = (lights[j]->position - hit);
		//set minimum to infinity, if it stays infinity then there are no intersections
		Hit min_shadow_hit(std::numeric_limits<double>::infinity(), Vector());
		//Hit min_shadow_hit(testStuff.length(), Vector());
		bool in_shadow = false;
		// Find hit object and distance
		for (unsigned int i = 0; i < objects.size(); i++) {
			Hit shadowHit(objects[i]->intersect(shadowTest));
			if (shadowHit.t < min_shadow_hit.t) { //&& shadowHit.t > 0
				//if (objects[i] != obj) //in case the line intersects the object because it is slightly inside it
				//{
					//set the minimum distance to the object being hit
				min_shadow_hit = shadowHit;
				in_shadow = true;
				//}
			}
		}
		//end of new shadow stuff

		ambient += material->ka * material->color; //ambient
		//if the point is not in a shadow
		if (in_shadow == false)
		{
			diffuse += material->kd * material->color * lights[j]->color * max(0.0, L.dot(N)); //diffuse
			specular += material->ks * lights[j]->color * pow(max(0.0, R.dot(V)), 20); //specular
		}
	}
	reflectIteration = reflectIteration - 1;
	if (reflectIteration > 0)
	{
		//The reflection calculation
		Vector reflectDirect = ray.D - (2 * (ray.D.dot(N))*N);
		Ray reflect(hit + (reflectDirect*0.01), reflectDirect);
		reflective += trace(reflect, reflectIteration, refractIteration, inside)*material->reflect;
	}
	//refraction calculation
	//assuming air index=1.00
	if (material->refract > 0.0)
	{
		refractIteration = refractIteration - 1;
		if (refractIteration > 0)
		{
			double index1 = 0;
			double index2 = 0;
			Vector T;
			//going in
			index1 = 1.00;
			index2 = material->eta;
			inside = true;
			double eta = index1 / index2;

			double c1 = -ray.D.dot(N);
			double c2_s = 1.0 - eta * eta * (1 - c1 * c1);

			if (c2_s >= 0)
			{
				T = eta * ray.D + (eta * c1 - sqrt(c2_s)) * N;
				Ray refract(hit + (T*0.01), T);
				Hit refractHit(obj->intersect(refract));

				if (refractHit.t > 0)
				{
					Point refractPT = refract.at(refractHit.t);                 //the hit point
					Vector rN = refractHit.N;                          //the normal at hit point
																	   //Vector V = -refract.D;     //the view vector

																	   //coming out
					index2 = 1.00;
					index1 = material->eta;

					eta = index1 / index2;

					c1 = -T.dot(-rN);
					c2_s = 1.0 - eta * eta * (1 - c1 * c1);

					if (c2_s >= 0)
					{
						T = eta * T + (eta * c1 - sqrt(c2_s)) * (-rN);
						Ray refractOut(refractPT + (T*0.01), T);
						/*double eta = index1 / index2;

						double c1 = -ray.D.dot(N);
						double c2_s = 1.0 - eta * eta * (1 - c1 * c1);

						if (c2_s >= 0)
						{
						Vector T = eta * ray.D + (eta * c1 + sqrt(c2_s)) * N;
						}*/
						//Vector refractDirect = ((index1*(ray.D - N*(ray.D.dot(N))) / index2) - N * (sqrt(1 - ((index1*index1)*(1 - ((ray.D.dot(N)*(ray.D.dot(N)))))) / (index2*index2))));
						//refractDirect = refractDirect.normalized();
						//Ray refract(hit + (refractDirect*0.01), refractDirect);
						//refractive += trace(refract, reflectIteration, refractIteration, inside)*material->refract;
						//T = T.normalized();
						//Ray refract(hit + (T*0.01), T);
						refractive += trace(refractOut, reflectIteration, refractIteration, inside)*material->refract;
					}


				}
			}

		}
	}

	//the color is equal to all 3 types of light added together
	color = ambient + diffuse + specular + reflective + refractive;

    return color;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h-1-y, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 5, 5, false);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
