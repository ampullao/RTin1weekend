#include "RTWeekend.h"

#include "Camera.h"
#include "Color.h"
#include "Hittable_List.h"
#include "Material.h"
#include "Sphere.h"
#include "bvh.h"
#include "texture.h"

void random_spheres() {
    hittable_list world;

    // auto ground_material = make_shared<lambertian>(Color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(Point3(0, -1000, 0), 1000, ground_material));
    auto checker = make_shared<checker_texture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    world.add(make_shared<sphere>(Point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center1 = center + Vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center1, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(Point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = Point3(13, 2, 3);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);
    
    cam.defocus_angle = 0.02;
    cam.focus_dist = 10.0;

    cam.render(world);
}

void two_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.8, Color(.2, .3, .1), Color(.9, .9, .9));

    world.add(make_shared<sphere>(Point3(0,-10,0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(Point3(0, 10,0), 10, make_shared<lambertian>(checker)));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = Point3(13, 2, 3);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("../texture/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(Point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio = 16. / 9.;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = Point3(0, 0, 12);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe));
}

void two_perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(Point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(Point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = Point3(13, 2, 3);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    switch(4) {
        case 1: random_spheres();     break;
        case 2: two_spheres();        break;
        case 3: earth();              break;
        case 4: two_perlin_spheres(); break;
    }
}