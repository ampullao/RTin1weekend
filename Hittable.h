#ifndef HITTABLE_H
#define HITTABLE_H

#include "RTWeekend.h"
#include "Ray.h"
#include "aabb.h"

class material;

class hit_record {
    public:
        Point3 p;
        Vec3 normal;
        shared_ptr<material> mat;
        double t;
        bool front_face;

        void set_face_normal(const Ray& r, const Vec3& outward_normal) {
            // Sets the hit record normal vector.
            // Note: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const Ray& r, interval ray_t, hit_record& rec) const = 0;

        virtual aabb bounding_box() const = 0;
};

#endif