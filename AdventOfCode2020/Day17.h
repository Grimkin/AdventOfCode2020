#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <list>
#include <array>
#include <span>

template<uint8_t numDims>
class ConwaysCube
{
public:
    ConwaysCube( std::string path );
    bool isInside( const std::array<int64_t, numDims>& pos );
    auto at( const std::array<int64_t, numDims>& pos );
    std::optional<bool> atOpt( const std::array<int64_t, numDims>& pos );
    void iterate();
    void iterate( int64_t num );
    int64_t getNumSet();
    void print();

private:
    void increase( const std::array<bool, numDims>& edge );
    void setEdges( const std::array<int64_t, numDims>& pos, std::array<bool, numDims>& edges );

    int getNumNeighbors( const std::array<int64_t, numDims>& pos );

    std::vector<bool> m_data;
    std::array<int64_t, numDims> m_sizes;
};

template<uint8_t numDims>
inline int64_t getNumActiveCubes( ConwaysCube<numDims>& cube, int64_t iter )
{
    cube.iterate( iter );
    return cube.getNumSet();
}

inline void Day17()
{
    ConwaysCube<3> cube( "Day17Input.txt" );
    ConwaysCube<4> hyperCube( "Day17Input.txt" );

    std::cout << "Day 17:\n";
    std::cout << "Part1: Number of cubes in cube after 6 iterations: " << getNumActiveCubes( cube, 6 ) << "\n";
    std::cout << "Part1: Number of cubes in hypercube after 6 iterations: " << getNumActiveCubes( hyperCube, 6 ) << "\n";
}

template<uint8_t numDims>
ConwaysCube<numDims>::ConwaysCube( std::string path )
{
    static_assert( numDims > 2 );

    std::ifstream file( path );
    size_t sizeX = 0;
    size_t sizeY = 0;
    for( std::string line; std::getline( file, line ); sizeY++ ) {
        sizeX = line.size();
        std::transform( begin( line ), end( line ), std::back_inserter( m_data ), [] ( char c ) { return c == '#'; } );
    }

    m_sizes[0] = sizeX;
    m_sizes[1] = sizeY;
    for( size_t i = 2; i < numDims; i++ ) {
        m_sizes[i] = 1;
    }

    std::array<bool, numDims> set;
    for( size_t i = 0; i < numDims; i++ ) {
        set[i] = true;
    }
    increase( set );
}


namespace
{
    template<uint8_t n>
    std::array<int64_t, n + 1> getIndices( int64_t newIdx, std::array<int64_t, n> oldIndices )
    {
        std::array<int64_t, n + 1> newIndices;
        newIndices[0] = newIdx;
        for( size_t i = 0; i < n; i++ ) {
            newIndices[i + 1] = oldIndices[i];
        }
        return newIndices;
    }
    template<uint8_t dim, uint8_t numIdx, typename Func>
    void iterate( std::span<int64_t, dim> sizes, std::array<int64_t, numIdx> indices, Func func )
    {
        for( size_t i = 0; i < sizes[dim - 1]; i++ ) {
            if constexpr( dim == 1 )
                func( getIndices( i, indices ) );
            else
                iterate<dim - 1>( sizes.subspan<0, dim - 1>(), getIndices( i, indices ), func );
        }
    }

    template<uint8_t dim, typename Func>
    void iterate( std::span<int64_t, dim> sizes, Func func )
    {
        iterate<dim, 0>( sizes, {}, func );
    }
}

template<uint8_t numDims>
bool ConwaysCube<numDims>::isInside( const std::array<int64_t, numDims>& pos )
{
    for( size_t i = 0; i < numDims; i++ ) {
        if( pos[i] < 0 || pos[i] >= m_sizes[i] )
            return false;
    }
    return true;
}

namespace
{
    template<uint8_t numDims>
    inline int64_t getIdx( const std::array<int64_t, numDims>& pos, const std::array<int64_t, numDims>& sizes )
    {
        int64_t idx = 0;
        int64_t offset = 1;

        for( size_t i = 0; i < numDims; i++ ) {
            idx += pos[i] * offset;
            offset *= sizes[i];
        }
        return idx;
    }
}

template<uint8_t numDims>
auto ConwaysCube<numDims>::at( const std::array<int64_t, numDims>& pos )
{
    return m_data[getIdx( pos, m_sizes )];
}

template<uint8_t numDims>
std::optional<bool> ConwaysCube<numDims>::atOpt( const std::array<int64_t, numDims>& pos )
{
    if( !isInside( pos ) )
        return {};
    return at( pos );
}


template<uint8_t numDims>
void ConwaysCube<numDims>::iterate()
{
    std::vector<bool> newData( m_data.size() );

    std::array<bool, numDims> edges{ false };

    ::iterate<numDims>( m_sizes,
        [this, &newData, &edges] ( const std::array<int64_t, numDims>& pos ) {
            int numNeighbors = getNumNeighbors( pos );
            auto cube = at( pos );
            if( numNeighbors == 3 || cube && numNeighbors == 2 ) {
                newData[getIdx( pos, m_sizes )] = true;;

                setEdges( pos, edges );
            }
        } );

    std::swap( m_data, newData );

    if( std::accumulate( begin( edges ), end( edges ), false, [] ( bool res, bool edge ) { return res || edge; } ) )
        increase( edges );
}

template<uint8_t numDims>
void ConwaysCube<numDims>::iterate( int64_t num )
{
    for( size_t i = 0; i < num; i++ ) {
        iterate();
    }
}

template<uint8_t numDims>
inline int64_t ConwaysCube<numDims>::getNumSet()
{
    int64_t numSet = 0;
    ::iterate<numDims>( m_sizes,
        [&] ( const std::array<int64_t, numDims>& pos ) {
            numSet += m_data[getIdx( pos, m_sizes )];
        } );
    return numSet;
}

template<uint8_t numDims>
inline void ConwaysCube<numDims>::print()
{
    for( int64_t z = 0; z < m_sizes[2]; z++ ) {
        for( int64_t y = 0; y < m_sizes[1]; y++ ) {
            for( int64_t x = 0; x < m_sizes[0]; x++ ) {
                std::cout << ( m_data[getIdx( { x,y,z }, m_sizes )] ? '#' : '.' );
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

template<uint8_t numDims>
inline void ConwaysCube<numDims>::setEdges( const std::array<int64_t, numDims>& pos, std::array<bool, numDims>& edges )
{
    for( size_t i = 0; i < numDims; i++ ) {
        if( pos[i] == 0 || pos[i] == m_sizes[i] - 1 )
            edges[i] = true;
    }
}

template<uint8_t numDims>
void ConwaysCube<numDims>::increase( const std::array<bool, numDims>& pointOnEdge )
{
    std::array<int64_t, numDims> newSizes;
    for( size_t i = 0; i < numDims; i++ ) {
        newSizes[i] = m_sizes[i] + ( pointOnEdge[i] ? 2 : 0 );
    }

    std::array<int64_t, numDims> offsets;
    for( size_t i = 0; i < numDims; i++ ) {
        offsets[i] = ( pointOnEdge[i] ? 1 : 0 );
    }
    auto getOffsetPos = [] ( const std::array<int64_t, numDims>& pos, const std::array<int64_t, numDims>& offset ) {
        std::array<int64_t, numDims> newPos;
        for( size_t i = 0; i < numDims; i++ ) {
            newPos[i] = pos[i] + offset[i];
        }
        return newPos;
    };
    std::vector<bool> newData( std::accumulate( begin( newSizes ), end( newSizes ), 1, [] ( int64_t prod, int64_t size ) { return prod * size; } ) );
    ::iterate<numDims>( m_sizes,
        [&] ( const std::array<int64_t, numDims>& pos ) {
            newData[getIdx( getOffsetPos( pos, offsets ), newSizes )] = m_data[getIdx( pos, m_sizes )];
        } );

    m_sizes = newSizes;

    std::swap( newData, m_data );
}

template<uint8_t numDims>
int ConwaysCube<numDims>::getNumNeighbors( const std::array<int64_t, numDims>& pos )
{
    int numNeighbors = 0;

    auto isMiddle = [] ( const std::array<int64_t, numDims>& pos ) {
        for( size_t i = 0; i < numDims; i++ ) {
            if( pos[i] != 1 )
                return false;
        }
        return true;
    };
    auto getOffsetPos = [] ( const std::array<int64_t, numDims>& pos, const std::array<int64_t, numDims>& offset ) {
        std::array<int64_t, numDims> newPos;
        for( size_t i = 0; i < numDims; i++ ) {
            newPos[i] = pos[i] + offset[i] - 1;
        }
        return newPos;
    };

    std::array<int64_t, numDims> neighbors;
    for( size_t i = 0; i < numDims; i++ )
        neighbors[i] = 3;

    ::iterate<numDims>( neighbors,
        [&] ( const std::array<int64_t, numDims>& offs ) {
            if( isMiddle( offs ) )
                return;
            numNeighbors += atOpt( getOffsetPos( pos, offs ) ).value_or( false );
        } );

    return numNeighbors;
}
