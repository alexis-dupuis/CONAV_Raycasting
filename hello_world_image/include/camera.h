//============================================================
// Adapted from "Raycasting in One Weekend" by Peter Shirley.
//============================================================

#ifndef CAMERAH
#define CAMERAH
#include "ray.h"




////////////////////////////////////////////////////////////////////
// Chapter 11:
vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),0) - vec3(1,1,0);
    } while (dot(p,p) >= 1.0);
    return p;
}
////////////////////////////////////////////////////////////////////


class camera {
    public:
        //Chapter 1-9:
		camera(){
			lower_left_corner = vec3( -2.0, -1.0, -1.0 );
			horizontal = vec3( 4.0, 0.0, 0.0 );
			vertical = vec3( 0.0, 2.0, 0.0 );
			origin = vec3( 0.0, 0.0, 0.0 );
		}
        
        // Chapter 10.1:
        camera(float vfov, float aspect){
            
            float theta = vfov*M_PI/180;
            float half_height = tan(theta/2);
            float half_width = aspect * half_height;
            
			lower_left_corner = vec3( -half_width, -half_height, -1.0 ); //"screen" at z = -1 in camera's frame
			horizontal = vec3( 2*half_width, 0.0, 0.0 );
			vertical = vec3( 0.0, 2*half_height, 0.0 );
			origin = vec3( 0.0, 0.0, 0.0 );
		}
        
        // Chapter 10.2:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) { // vfov is top to bottom in degrees
                        
            float theta = vfov*M_PI/180;
            float half_height = tan(theta/2);
            float half_width = aspect * half_height;
            
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            
            lower_left_corner = origin  - half_width*u -half_height*v - w;
            horizontal = 2*half_width*u;
            vertical = 2*half_height*v;
        }
        
		// Chapter 11:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) { // vfov is top to bottom in degrees
            lens_radius = aperture / 2;
            float theta = vfov*M_PI/180;
            float half_height = tan(theta/2);
            float half_width = aspect * half_height;
            origin = lookfrom;
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
            lower_left_corner = origin  - half_width*focus_dist*u -half_height*focus_dist*v - focus_dist*w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }
				
        /* // Chapters 6+:
		ray get_ray(float s, float t){ 
			return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
		}*/
		
        //Chapter 11:
        ray get_ray(float s, float t) {
            vec3 rd = lens_radius*random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset); 
        }

        vec3 origin; // camera frame origin
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w; // camera frame vectors
        float lens_radius;
};
#endif




