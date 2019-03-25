#include <fstream>
#include <iostream>
#include "vec3.h" //already in ray.h
#include "ray.h" //already in hitable.h which is in sphere.h and hitable_list.h
#include "sphere.h"
#include "hitable_list.h"
#include "float.h" //for FLT_MAX
#include "camera.h"

using namespace std;


// float/*Ch4: bool*/ hit_sphere(const vec3& center, float radius, const ray& r){
	
	// vec3 oc = r.origin() - center;
	
	// float a = dot( r.direction(), r.direction() );
	// float b = 2.0 * dot( oc, r.direction() );
	// float c = dot( oc, oc ) - radius*radius;
	
	// float discriminant = b*b - 4*a*c;
	
	// /* //Chapter 4:
	// return (discriminant > 0); */
	
	//Chapter 5:
	// if (discriminant < 0){
		// return -1.0; // no contact point...
	// }
	// else{
		// return (-b - sqrt(discriminant) ) / (2.0*a); // the ray/sphere contact point (the closest one)
	// }
// }

/* //Chapter 7: moved to material.h after that
vec3 random_in_unit_sphere(){
	
	vec3 p;
	do{
		p = 2.0*vec3( drand48(), drand48(), drand48() ) - vec3(1,1,1);
	} while( p.squared_length() >= 1.0 );
	return p;
	
}*/


vec3 color(const ray& r, hitable *world, /*Chapter 8+*/int depth){
	
	/* //Chapter 4:
	if( hit_sphere( vec3(0,0,-1), 0.5, r) ){ //sphere of radius 0.5 centered in (0,0,-1)
		return vec3(1,0,0); //red pixel (for a red sphere)
	}*/
	
	/* //Chapter 5.1:
	float t = hit_sphere( vec3(0,0,-1), 0.5, r );
	if( t > 0.0 ){
		vec3 N = unit_vector( r.point_at_parameter(t) - vec3(0,0,-1) ); //normal to the sphere at p(t)
		return 0.5*vec3( N.x()+1, N.y()+1, N.z()+1 ); //mapped to [0,1] and used as a color for illustration purposes
	}
	*/
	
	/* //Chapter 5.2 - 6:
	hit_record rec;
	if( world->hit( r, 0.0, FLT_MAX, rec ) ){
			return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1); //mapped to [0,1] to make a color
	}
	*/
	
	/* //Chapter 7
	hit_record rec;
	if( world->hit( r, 0.001, FLT_MAX, rec ) ){
			vec3 target = rec.p + rec.normal + random_in_unit_sphere(); //direction of reflected ray
			return 0.5 * color( ray(rec.p, target - rec.p), world);
	}*/
	
	//Chapter 8
	hit_record rec;
	if( world->hit( r, 0.001, FLT_MAX, rec ) ){
		
		ray scattered;
		vec3 attenuation;
		
		if( depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered) ){
			return attenuation * color(scattered, world, depth+1);			
		}
		else{
			return vec3(0,0,0);
		}
	}

	
	else{ //lerp
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
	}
}

// Chapter 12: a random scene generator
hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48()); 
            if ((center-vec3(4,0.2,0)).length() > 0.9) { 
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())),  0.5*drand48()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}


int main(){

	//image file...
	ofstream outfile;
	outfile.open("../output/final_picture.ppm", ios::out); // opens (and creates if non-existent) for output
	if( outfile.is_open() ){
		cout << "Image file opened. \n";
	}
	else{
		cout << "Error opening or creating image file. \n";
	}

	//number of samples
	int ns = 10; //100
	
	//image dimensions
	int nx = 1200; //200
	int ny = 800; //100
	
	outfile << "P3\n" << nx << " " << ny << "\n255\n";
	
	/* //Camera vectors //Chapters < 5
	vec3 lower_left_corner( -2.0, -1.0, -1.0 );
	vec3 horizontal( 4.0, 0.0, 0.0 );
	vec3 vertical( 0.0, 2.0, 0.0);
	vec3 origin( 0.0, 0.0, 0.0);*/

	/* //Chapter 5+ //Hitables in the world
	hitable *list[4];
	list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)) );
	list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)) );
	list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3) );
	//list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 1.0) );
	list[3] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5) );//radius < 0 = hollow sphere

	hitable *world = new hitable_list(list,4); //4 spheres...
	*/
    
    //Chapter 12: a whole new world...
    hitable *list[5];
    float R = cos(M_PI/4);
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list,5);
    world = random_scene();
	
	//Chapter 6+ //Defining a camera
    //camera cam; //Chapter 6-9
	//camera cam(140, float(nx)/float(ny)); //Chapter10.1
    //camera cam(vec3(-2,2,1), vec3(0,0,-1), vec3(0,1,0), 30, float(nx)/float(ny)); //Chapter 10.2
	//camera cam(vec3(3,3,2), vec3(0,0,-1), vec3(0,1,0), 20, float(nx)/float(ny), 2.0, vec3(3,3,3).length()); //Chapter 11
	camera cam(vec3(13,2,3), vec3(0,0,0), vec3(0,1,0), 20, float(nx)/float(ny), 0.1, 10); //Chapter 11
    
	//rendering loop
	for( int j = ny-1; j >= 0; j-- ){
		for( int i = 0; i < nx; i++ ){
			
			/* //Chapter 1:
			float r = float(i) / float(nx);
			float g = float(j) / float(ny);
			float b = 0.2;
			
			int ir = int(255.99 * r);
			int ig = int(255.99 * g);
			int ib = int(255.99 * b);
			*/
			
			/*//Chapter 2:
			vec3 col( float(i) / float(nx), float(j) / float(ny), 0.2);
			*/
			
			/* //Chapter 3-4-5.1:
			vec3 col = color(r);*/
			
			/*
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);
			ray r( origin, lower_left_corner + u*horizontal + v*vertical );*/
			
			/* //Chapter 5.2:
			vec3 p = r.point_at_parameter(2.0);
			vec3 col = color(r, world);
			*/
			
			//Chapter 6:
			vec3 col(0,0,0);
			for(int s = 0; s < ns; s++){
				
				float u = float( i + drand48() ) / float(nx); //random sample placement
				float v = float( j + drand48() ) / float(ny);
				
				ray r = cam.get_ray(u,v);
				vec3 p = r.point_at_parameter(2.0);
				
				col += color( r, world, 0 );//no last arg for chapter <8

			}
			col /= float(ns);
			
			//Chapter 7: raising color to gamma 2
			col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );

			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	
	outfile.close();// closes file;
	cout << "Image rendered succesfully." << endl;
	return 0;
}
