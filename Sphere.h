#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Vec3.h"

class sphere : public hittable {
    public:
        // Stationaty Sphere
        sphere(Point3 _center, double _radius, shared_ptr<material> _material)
            : center0(_center), radius(_radius), mat(_material), is_moving(false)
        {
            auto rvec = Vec3(radius, radius, radius);
            bbox = aabb(center0 - rvec, center0 + rvec);
        }
        
        // Moving Sphere
        sphere(Point3 _center0, Point3 _center1, double _radius, shared_ptr<material> _material)
            : center0(_center0), radius(_radius), mat(_material), is_moving(true)
        {
            auto rvec = Vec3(radius, radius, radius);
            aabb box0(_center0 - rvec, _center0 + rvec);
            aabb box1(_center1 - rvec, _center1 + rvec);
            bbox = aabb(box0, box1);

            center_vec = _center1 - _center0;
        }

        bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
            Point3 center = is_moving ? sphere_center(r.time()) : center0;
            Vec3 oc = r.origin() - center;
            auto a = r.direction().length_squared();
            auto half_b = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius * radius;

            auto discriminant = half_b * half_b - a * c;
            if (discriminant < 0)
                return false;
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (-half_b + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            Vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat = mat;

            return true;
        }

        aabb bounding_box() const override { return bbox; }
    
    private:
        Point3 center0;
        double radius;
        shared_ptr<material> mat;
        bool is_moving;
        Vec3 center_vec;
        aabb bbox;

        Point3 sphere_center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0
            // yields center0, and t=1 yields center1.
            return center0 + time * center_vec;
        }

        static void get_sphere_uv(const Point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.50>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2 * pi);
            v = theta / pi;
        }
};

#endif