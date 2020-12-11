#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>
#include <span>

enum class Seat
{
    None,
    Free,
    Taken
};

std::ostream& operator<<( std::ostream& stream, Seat seat )
{
    switch( seat ) {
    case Seat::None:
        return stream << '.';
        break;
    case Seat::Free:
        return stream << 'L';
    case Seat::Taken:
        return stream << '#';
    }
    throw std::runtime_error( "invalid seat type" );
}

template<typename T>
class Matrix
{
public:
    Matrix( int numCols, int numRows, std::vector<T> data ) : m_data( std::move( data ) ), m_numCols( numCols ), m_numRows( numRows ) {}
    T& at( int col, int row ) { return m_data[col + row * m_numCols]; }
    const T& at( int col, int row ) const { return m_data[col + row * m_numCols]; }
    std::optional<T> optAt( int col, int row ) const { if( col < 0 || col >= m_numCols || row < 0 || row >= m_numRows ) return {}; return at( col, row ); }
    int getNumRows() const { return m_numRows; }
    int getNumCols() const { return m_numCols; }
    void print()
    {
        for( size_t row = 0; row < m_numRows; row++ ) {
            for( size_t col = 0; col < m_numCols; col++ ) {
                std::cout << at( col, row );
            }
            std::cout << "\n";
        }
    }
    bool isInside( int col, int row ) const { return col >= 0 && col < m_numCols&& row >= 0 && row < m_numRows; }

private:
    std::vector<T> m_data;
    int m_numCols;
    int m_numRows;
};

Seat charToSeat( char c )
{
    switch( c ) {
    case 'L':
        return Seat::Free;
    case '.':
        return Seat::None;
    case '#':
        return Seat::Taken;
    }
    throw std::runtime_error( "invalid seat char" );
}

void insertLine( std::vector<Seat>& data, const std::string& line )
{
    std::transform( begin( line ), end( line ), std::back_inserter( data ), &charToSeat );
}

Matrix<Seat> getData( const std::string& path )
{
    std::ifstream file( path );

    std::vector<Seat> data;
    int numColumns = 0;
    int numRows = 0;
    for( std::string line; std::getline( file, line ); ) {
        numColumns = line.size();
        insertLine( data, line );
        numRows++;
    }
    return { numColumns, numRows, std::move( data ) };
}

int getNumSurrounding( const Matrix<Seat>& seats, int column, int row )
{
    int numSeatsTaken = 0;
    for( auto& rowOffs : { -1,0,1 } ) {
        for( auto& colOffs : { -1,0,1 } ) {
            if( rowOffs == 0 && colOffs == 0 )
                continue;
            numSeatsTaken += seats.optAt( column + colOffs, row + rowOffs ).value_or( Seat::None ) == Seat::Taken;
        }
    }
    return numSeatsTaken;
}

bool applyStep( Matrix<Seat>& targetSeats, const Matrix<Seat>& srcSeats )
{
    bool changed = false;
    for( int row = 0; row < srcSeats.getNumRows(); row++ ) {
        for( int col = 0; col < srcSeats.getNumCols(); col++ ) {
            int numNeighbors = getNumSurrounding( srcSeats, col, row );
            if( numNeighbors == 0 && srcSeats.at( col, row ) == Seat::Free ) {
                targetSeats.at( col, row ) = Seat::Taken;
                changed = true;
            }
            else if( numNeighbors >= 4 && srcSeats.at( col, row ) == Seat::Taken ) {
                targetSeats.at( col, row ) = Seat::Free;
                changed = true;
            }
            else
                targetSeats.at( col, row ) = srcSeats.at( col, row );
        }
    }
    return changed;
}

int numSeatsTaken( const Matrix<Seat>& seatData )
{
    int seatsTaken = 0;
    for( int row = 0; row < seatData.getNumRows(); row++ ) {
        for( int col = 0; col < seatData.getNumCols(); col++ ) {
            seatsTaken += seatData.at( col, row ) == Seat::Taken;
        }
    }

    return seatsTaken;
}

int getFinalNumSeatsTaken( Matrix<Seat> seatData )
{
    auto tmpSeats = seatData;
    while( applyStep( seatData, tmpSeats ) ) {
        std::swap( seatData, tmpSeats );
    }

    return numSeatsTaken( seatData );
}

struct Vec2
{
    int x, y;
};

Vec2& operator+=( Vec2& a, const Vec2& b )
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

bool checkDirection( const Matrix<Seat>& seatData, Vec2 pos, Vec2 dir )
{
    pos += dir;
    auto seat = seatData.optAt( pos.x, pos.y );
    if( !seat )
        return false;

    if( *seat == Seat::Taken )
        return true;
    if( *seat == Seat::Free )
        return false;

    return checkDirection( seatData, pos, dir );
}

int getNumSeenSeats( const Matrix<Seat>& seatData, Vec2 pos )
{
    int numSeenSeats = 0;
    for( auto& y : {-1,0,1} ) {
        for( auto& x : {-1,0,1} ) {
            if( x == 0 && y == 0 )
                continue;
            numSeenSeats += checkDirection( seatData, pos, { x,y } );
        }
    }
    return numSeenSeats;
}

bool applyStepDir( Matrix<Seat>& targetSeats, const Matrix<Seat>& srcSeats )
{
    bool changed = false;
    for( int row = 0; row < srcSeats.getNumRows(); row++ ) {
        for( int col = 0; col < srcSeats.getNumCols(); col++ ) {
            if( srcSeats.at( col, row ) == Seat::None )
                continue;

            int numSeenSeets = getNumSeenSeats( srcSeats, { col, row } );
            if( numSeenSeets == 0 && srcSeats.at( col, row ) == Seat::Free ) {
                targetSeats.at( col, row ) = Seat::Taken;
                changed = true;
            }
            else if( numSeenSeets >= 5 && srcSeats.at( col, row ) == Seat::Taken ) {
                targetSeats.at( col, row ) = Seat::Free;
                changed = true;
            }
            else
                targetSeats.at( col, row ) = srcSeats.at( col, row );
        }
    }
    return changed;
}

int getFinalNumSeatsTakenDir( Matrix<Seat> seatData )
{
    auto tmpSeats = seatData;
    while( applyStepDir( seatData, tmpSeats ) ) {
        std::swap( seatData, tmpSeats );
    }

    return numSeatsTaken( seatData );
}

void Day11()
{
    auto data = getData( "Day11Input.txt" );

    std::cout << "Day 11:\n";
    std::cout << "Part1: final number of seats taken: " << getFinalNumSeatsTaken( data ) << "\n";
    std::cout << "Part2: final number of seats taken: " << getFinalNumSeatsTakenDir( data ) << "\n";
}
