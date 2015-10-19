/**
 * @file heightmap.cpp
 * @brief Backend for the heightmap.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 */


#include "scene/heightmap.hpp"


namespace _462 {

WaterSurface::WaterSurface() : current_time( 0 ) { }

WaterSurface::~WaterSurface() { }

real_t WaterSurface::compute_height( const Vector2& pos ) const
{
    assert( pos.x >= -1.0 && pos.x <= 1.0 );
    assert( pos.y >= -1.0 && pos.y <= 1.0 );

    real_t h = 0;

    for ( size_t i = 0; i < wave_points.size(); ++i ) {
        const WavePoint& p = wave_points[i];
        real_t r = distance( pos, p.position );
        h += p.amplitude * exp( -p.falloff * r ) * sin( p.period * r + p.timerate * current_time );
    }

    return h;
}


void WaterSurface::update( real_t dt )
{
    current_time += dt;
}


Vector3 WaterSurface::compute_normal(const Vector2 &pos) const
{
    assert( pos.x >= -1.0 && pos.x <= 1.0 );
    assert( pos.y >= -1.0 && pos.y <= 1.0 );

    real_t dfdx = 0;
    real_t dfdy = 0;
    for ( size_t i = 0; i < wave_points.size(); ++i )
    {
        const WavePoint& p = wave_points[i];
        real_t r = distance( pos, p.position );
        real_t k = p.amplitude * exp(-p.falloff *r)*(p.period*(-p.falloff)*std::log(p.amplitude))
                   *cos(p.period * r + p.timerate * current_time )/(r*r) ;
        dfdx += k*(pos.x-p.position.x)*(pos.x-p.position.x);
        dfdy += k*(pos.y-p.position.y)*(pos.y-p.position.y);
    }
    return normalize(Vector3(-dfdx, -dfdy, 1));
}



} /* _462 */

