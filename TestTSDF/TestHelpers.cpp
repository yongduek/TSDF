//
//  TestHelpers.cpp
//  KinFu
//
//  Created by Dave on 16/05/2016.
//  Copyright © 2016 Sindesso. All rights reserved.
//

#include <stdio.h>
#include "TestHelpers.hpp"
#include "PgmUtilities.hpp"
#include "PngUtilities.hpp"


#pragma mark - helpers

phd::TSDFVolume construct_volume( int vx, int vy, int vz, float px, float py, float pz, float & vw, float & vh, float & vd) {
    Eigen::Vector3i size{ vx, vy, vz };
    Eigen::Vector3f physical_size{ px, py, pz };
    phd::TSDFVolume volume{ size, physical_size };
    
    vw = px / vx;
    vh = py / vy;
    vd = pz / vz;
    
    return volume;
}


void create_sphere_in_TSDF( phd::TSDFVolume & volume, float radius ) {
    using namespace phd;
    using namespace Eigen;
    
    Vector3i size = volume.size();
    Vector3f volume_centre = volume.centre_of_volume();
    
    float trunc_dist = volume.truncation_distance();
    
    for( int i=0; i<size.x(); i++ ) {
        for( int j=0; j<size.y(); j++ ) {
            for( int k=0; k<size.z(); k++ ) {
                Vector3f voxel_centre = volume.centre_of_voxel_at(i, j, k);
                Vector3f vector = volume_centre - voxel_centre;
                float dist = std::sqrt( vector.dot( vector ) );
                dist = dist - radius;
                
                dist = std::fminf( std::fmaxf( dist, -trunc_dist ), trunc_dist );
                
                volume.set_distance(i, j, k, dist);
            }
        }
    }
}

void create_wall_in_TSDF( phd::TSDFVolume & volume, float depth ) {
    using namespace phd;
    using namespace Eigen;
    
    float trunc_dist = volume.truncation_distance();

    Vector3i size = volume.size();
    
    for( int i=0; i<size.x(); i++ ) {
        for( int j=0; j<size.y(); j++ ) {
            for( int k=0; k<size.z(); k++ ) {
                Vector3f voxel_centre = volume.centre_of_voxel_at(i, j, k);
                
                float dist = depth - voxel_centre.z();
                
                dist = std::fminf( std::fmaxf( dist, -trunc_dist ), trunc_dist );
                volume.set_distance(i, j, k, dist);
            }
        }
    }
}

void create_cube_in_TSDF( phd::TSDFVolume & volume, float depth ) {
    using namespace phd;
    using namespace Eigen;
    
    float trunc_dist = volume.truncation_distance();
    
    Vector3i size = volume.size();
    
    for( int i=0; i<size.x(); i++ ) {
        for( int j=0; j<size.y(); j++ ) {
            for( int k=0; k<size.z(); k++ ) {
                Vector3f voxel_centre = volume.centre_of_voxel_at(i, j, k);
                
                float dist = depth - voxel_centre.z();
                
                dist = std::fminf( std::fmaxf( dist, -trunc_dist ), trunc_dist );
                volume.set_distance(i, j, k, dist);
                volume.set_weight( i, j, k, 1 );
            }
        }
    }
}

void save_normals_as_png( std::string filename, uint16_t width, uint16_t height, Eigen::Vector3f * normals ) {
    // Allocate grey scale
    uint8_t * image = new uint8_t[width * height];
    
    Eigen::Vector3f axis{ 0.577f, 0.577f, 0.577f };
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        
        Eigen::Vector3f n = normals[idx];
        float d = n.dot( axis);
        
        image[idx] = std::floorf(d * 255);
    }
    
    save_png_to_file(filename, width, height, image);
    
    delete[] image;
}
void save_normals_as_png( std::string filename, uint16_t width, uint16_t height, const std::deque<Eigen::Vector3f> &normals ) {
    // Allocate grey scale
    uint8_t * image = new uint8_t[width * height];
    
    Eigen::Vector3f axis{ 0.577f, 0.577f, 0.577f };
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        
        Eigen::Vector3f n = normals[idx];
        float d = n.dot( axis);
        
        image[idx] = std::floorf(d * 255);
    }
    
    save_png_to_file(filename, width, height, image);
    
    delete[] image;
}


void save_normals_as_colour_png( std::string filename, uint16_t width, uint16_t height, Eigen::Vector3f * normals ) {
    // Allocate grey scale
    uint8_t * image = new uint8_t[width * height * 3];
    
    uint32_t write_idx = 0;
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        
        Eigen::Vector3f n = normals[idx];
        
        if( n[2] < 0 ) n[2] = -n[2];
        n = ((n / 2.0f) + Eigen::Vector3f{0.5, 0.5, 0.5}) * 255;
        image[write_idx++] = floor( n[0] );
        image[write_idx++] = floor( n[1] );
        image[write_idx++] = floor( n[2] );
    }
    
    save_colour_png_to_file(filename, width, height, image);
    
    delete[] image;
}
void save_normals_as_colour_png( std::string filename, uint16_t width, uint16_t height, const std::deque<Eigen::Vector3f> &normals ) {
    // Allocate grey scale
    uint8_t * image = new uint8_t[width * height * 3];
    
    uint32_t write_idx = 0;
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        
        Eigen::Vector3f n = normals[idx];
        
        if( n[2] < 0 ) n[2] = -n[2];
        n = ((n / 2.0f) + Eigen::Vector3f{0.5, 0.5, 0.5}) * 255;
        image[write_idx++] = floor( n[0] );
        image[write_idx++] = floor( n[1] );
        image[write_idx++] = floor( n[2] );
    }
    
    save_colour_png_to_file(filename, width, height, image);
    
    delete[] image;
}


// Lambertian colouring as per https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
void save_rendered_scene_as_png(std::string filename, uint16_t width, uint16_t height, Eigen::Vector3f * vertices, Eigen::Vector3f * normals, const phd::Camera & camera, const Eigen::Vector3f & light_source) {
    
    uint8_t * image = new uint8_t[width * height];

    // Ensure that there's always ambient light
    float ambient_coefficient = 0.2;
    float diffuse_coefficient = 1.0 - ambient_coefficient;
    
    // convert the global light sourcr poition to camera space
    // ( Vertices and normals are in cam space )
    Eigen::Vector3f light_source_cam;
    camera.world_to_camera(light_source, light_source_cam );
    

    // For each vertex/normal
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        // Vertex in camera space
        Eigen::Vector3f vertex = vertices[idx];
        
        // Compute vector from vertex to light source
        Eigen::Vector3f r = (light_source_cam - vertex).normalized();
        Eigen::Vector3f n = normals[idx];
        
        // Compute shade
        float shade = std::fmax( 0.0, r.dot( n ) );
        shade = ambient_coefficient + ( diffuse_coefficient * shade );
        
        image[idx] = std::floorf( shade * 255  );
    }
    
    save_png_to_file(filename, width, height, image);
    
    delete[] image;
}
void save_rendered_scene_as_png(std::string filename, uint16_t width, uint16_t height, const std::deque<Eigen::Vector3f> &vertices, const std::deque<Eigen::Vector3f> &normals, const Eigen::Vector3f & camera_position, const Eigen::Vector3f & light_source) {
    
    uint8_t * image = new uint8_t[width * height];
    
    // Ensure that there's always ambient light
    float ambient_coefficient = 0.2;
    float diffuse_coefficient = 1.0 - ambient_coefficient;
    
    // For each vertex/normal
    for (uint32_t idx = 0; idx < (width*height); idx ++ ) {
        // Vertex in camera space
        Eigen::Vector3f vertex = vertices[idx];
        
        // Convert to global space
        vertex = vertex + camera_position;
        
        // Compute vector from light source to vertex
        Eigen::Vector3f r = (light_source - vertex).normalized();
        Eigen::Vector3f n = normals[idx];
        
        // Compute shade
        float shade = std::fmax( 0.0, r.dot( n ) );
        shade = ambient_coefficient + ( diffuse_coefficient * shade );
        
        image[idx] = std::floorf( shade * 255  );
    }
    
    save_png_to_file(filename, width, height, image);
    
    delete[] image;
}



uint16_t * read_tum_depth_map( const std::string & file_name, uint32_t & width, uint32_t & height ) {
    uint16_t * range_map = load_png_from_file( file_name, width, height );
    
    size_t map_size = width * height;
    for( size_t i=0; i<map_size; i++ ) {
        uint16_t v = range_map[i];
        
        // Convert to metres by dividing by 5000, then to millimetres by multiplying by 1000
        range_map[i] = v / 5;
    }
    
    return range_map;
}

// NYU Maps are in mm already but do need to be byte swapped
uint16_t * read_nyu_depth_map( const std::string & file_name, uint32_t & width, uint32_t & height ) {
    uint16_t * range_map = read_pgm( file_name, width, height );
    
    size_t map_size = width * height;
    for( size_t i=0; i<map_size; i++ ) {
        uint16_t v = range_map[i];
        
        v = (v >> 8) + ( ( v & 0xFF ) * 256 );
        
        range_map[i] = v;
    }
    
    return range_map;
}


Eigen::Matrix4f make_y_axis_rotation( float theta, Eigen::Vector3f pos ) {
    
    float cos_theta = cosf( theta );
    float sin_theta = sinf( theta );
    Eigen::Matrix4f rot;

    rot <<  cos_theta,  0,  sin_theta,  pos.x(),
            0,          1,  0,          pos.y(),
            -sin_theta, 0,  cos_theta,  pos.z(),
            0,          0,  0,          1;

    return rot;
}
Eigen::Matrix4f make_x_axis_rotation( float theta, Eigen::Vector3f pos ) {
    
    float cos_theta = cosf( theta );
    float sin_theta = sinf( theta );
    Eigen::Matrix4f rot;
    
    rot <<  1, 0,           0,          pos.x(),
            0, cos_theta,   -sin_theta, pos.y(),
            0, sin_theta,   cos_theta,  pos.z(),
            0,  0,          0,          1;
    
    return rot;
}
Eigen::Matrix4f make_z_axis_rotation( float theta, Eigen::Vector3f pos ) {
    
    float cos_theta = cosf( theta );
    float sin_theta = sinf( theta );
    Eigen::Matrix4f rot;
    
    rot <<  cos_theta, -sin_theta, 0, pos.x(),
            sin_theta,  cos_theta, 0, pos.y(),
            0,          0,         1, pos.z(),
            0,          0,         0,  1;
    
    return rot;
}

phd::Camera make_kinect( ) {
    return phd::Camera{ 585.6f, 585.6f, 316.0f, 247.6f };
}

uint16_t * make_sphere_depth_map( uint16_t width, uint16_t height, uint16_t radius, uint16_t max_depth, uint16_t min_depth ) {
    uint16_t * depths = new uint16_t[width*height];
    
    uint32_t idx = 0;
    float cx = width/2.0f;
    float cy = height/2.0f;
    float r2 = radius * radius;
    float depth_centre = (max_depth - min_depth) / 2.0;
    float dz_scale = (depth_centre * 4 / 250 );
    for( uint16_t y=0; y<height; y++ ) {
        for( uint16_t x=0; x<width; x++ ) {
            uint16_t depth = max_depth;

            float dx = cx - x;
            float dy = cy - y;
            float dx2 = dx*dx;
            float dy2 = dy*dy;
            if( dx2+dy2 < r2 ) {
                // dx^2 + dy^2 + dz^2 = radius^2
                float dz = std::sqrtf( r2 - (dx2+dy2) );
                depth = depth_centre - ( dz * dz_scale);
            }
            
            depths[idx] = std::max(min_depth, std::min( max_depth, depth ) );
            idx++;
        }
    }
    
    return depths;
}

uint16_t * make_wall_depth_map( uint16_t width, uint16_t height, uint16_t max_depth, uint16_t min_depth, uint16_t wall_depth ) {
    uint16_t * depths = new uint16_t[width*height];
    
    int wall_min_x = (width / 5) * 2;
    int wall_max_x = (width / 5) * 3;
    int wall_min_y = (height / 5) * 2;
    int wall_max_y = (height / 5) * 3;
    
    uint32_t idx = 0;
    for( uint16_t y=0; y<height; y++ ) {
        for( uint16_t x=0; x<width; x++ ) {
            uint16_t depth = max_depth;
            

            if( x > wall_min_x && x < wall_max_x && y > wall_min_y && y < wall_max_y ) {
                depth = wall_depth;
            }
            depths[idx] = std::max(min_depth, std::min( max_depth, depth ) );
            idx++;
        }
    }
    
    return depths;
}
