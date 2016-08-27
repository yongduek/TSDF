//
//  CPURaycaster.hpp
//  KinFu
//
//  Created by Dave on 16/06/2016.
//  Copyright © 2016 Sindesso. All rights reserved.
//

#ifndef CPURaycaster_hpp
#define CPURaycaster_hpp

#include <Eigen/Core>
#include "CPUTSDFVolume.hpp"

namespace phd {
class CPURaycaster {
public:
    CPURaycaster( uint16_t width=640, uint16_t height=480) :m_width{width}, m_height{height}{};

    /**
     * Compute the normal to the ISO surface at the given point
     * Based on http://www.cs.technion.ac.il/~veredc/openfusion/OpenFusionReport.pdf
     * @param point The point; should be inside the TSDF
     * @param normal The returned normal
     */
    void normal_at_point( const CPUTSDFVolume & volume, const Eigen::Vector3f & point, Eigen::Vector3f & normal ) const;

    /**
     * Find the point where the given ray first intersects the TSDF space in global coordinates
     * @param origin The source of the ray
     * @param ray_direction A unit vector in the direction of the ray
     * @param entry_point The point at which the ray enters the TSDF which may be the origin
     * @param t The ray parameter for the intersection; entry_point = origin + (t * ray_direction)
     * @return true if the ray intersects the TSDF otherwise false
     */
    bool is_intersected_by_ray( const Eigen::Vector3f & origin, const Eigen::Vector3f & ray_direction, Eigen::Vector3f & entry_point, float & t ) const;

    /**
     * Walk ray from start to end seeking intersection with the ISO surface in this TSDF
     * If an intersection is found, return the coordnates in vertex and the surface normal
     * in normal
     * @param volume The volume to be rendered
     * @param ray_start The origin of the ray to be traced
     * @param ray_directioon The direction of the ray to be traced
     * @param vertex The returned vertex
     * @param normal The returned normal
     * @return true if the ray intersects the ISOSurface in which case vertex and normal are populated or else false if not
     */
    bool walk_ray( const CPUTSDFVolume & volume, const Eigen::Vector3f & ray_start, const Eigen::Vector3f & ray_direction, Eigen::Vector3f & vertex, Eigen::Vector3f & normal) const;

    /**
     * Raycast the TSDF and store discovered vertices and normals in the ubput arrays
     * @param volume The volume to cast
     * @param camera The camera
     * @param vertices The vertices discovered
     * @param normals The normals
     */
    void raycast( const CPUTSDFVolume & volume, const Camera & camera, Eigen::Matrix<float, 3, Eigen::Dynamic> &  vertices, Eigen::Matrix<float, 3, Eigen::Dynamic> &  normals ) const;

private:
    uint16_t    m_width;
    uint16_t    m_height;
};
}
#endif /* CPURaycaster_hpp */