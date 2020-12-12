#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>
#include <map>

enum class InstructionType
{
    MoveNorth,
    MoveSouth,
    MoveEast,
    MoveWest,
    RotateLeft,
    RotateRight,
    MoveForward
};

enum class Orientation
{
    North,
    South,
    East,
    West
};

struct Instruction
{
    InstructionType instruction;
    int value;
};

struct Vec2
{
    int x = 0;
    int y = 0;
};

struct Ship
{
    Vec2 position;
    Orientation orientation = Orientation::East;
};

struct ShipWithWaypoint
{
    Vec2 position;
    Vec2 waypoint{ 10,1 };
};

InstructionType getInstructionType( char c )
{
    switch( c ) {
    case 'N':
        return InstructionType::MoveNorth;
    case 'S':
        return InstructionType::MoveSouth;
    case 'E':
        return InstructionType::MoveEast;
    case 'W':
        return InstructionType::MoveWest;
    case 'L':
        return InstructionType::RotateLeft;
    case 'R':
        return InstructionType::RotateRight;
    case 'F':
        return InstructionType::MoveForward;
    }
    throw std::runtime_error( "invalid instruction" );
}

Instruction parseInstruction( const std::string& line )
{
    static std::regex regex( R"((\w)(\d+))" );
    std::smatch match;
    if( !std::regex_match( line, match, regex ) )
        throw std::runtime_error( "invalid instrucion" );

    return { getInstructionType( match[1].str()[0] ), std::stoi( match[2] ) };
}

std::vector<Instruction> parseInstructions( const std::string& path )
{
    std::vector<Instruction> instructions;
    std::ifstream file( path );
    for( std::string line; std::getline( file, line ); ) {
        instructions.push_back( parseInstruction( line ) );
    }
    return instructions;
}

void rotateLeft( Ship& ship )
{
    switch( ship.orientation ) {
    case Orientation::North:
        ship.orientation = Orientation::West;
        return;
    case Orientation::South:
        ship.orientation = Orientation::East;
        return;
    case Orientation::East:
        ship.orientation = Orientation::North;
        return;
    case Orientation::West:
        ship.orientation = Orientation::South;
        return;
    }

    throw std::runtime_error( "invalid orientation" );
}

void rotateRight( Ship& ship )
{
    switch( ship.orientation ) {
    case Orientation::North:
        ship.orientation = Orientation::East;
        return;
    case Orientation::South:
        ship.orientation = Orientation::West;
        return;
    case Orientation::East:
        ship.orientation = Orientation::South;
        return;
    case Orientation::West:
        ship.orientation = Orientation::North;
        return;
    }

    throw std::runtime_error( "invalid orientation" );
}

void rotateLeft( Ship& ship, int value )
{
    while( value > 0 ) {
        rotateLeft( ship );
        value -= 90;
    }
}

void rotateRight( Ship& ship, int value )
{
    while( value > 0 ) {
        rotateRight( ship );
        value -= 90;
    }
}

void moveForward( Ship& ship, int value )
{
    switch( ship.orientation ) {
    case Orientation::North:
        ship.position.y += value;
        return;
    case Orientation::South:
        ship.position.y -= value;
        return;
    case Orientation::East:
        ship.position.x += value;
        return;
    case Orientation::West:
        ship.position.x -= value;
        return;
    }
    throw std::runtime_error( "invalid orientation" );
}

void executeInstruction( Ship& ship, const Instruction& instruction )
{
    switch( instruction.instruction ) {
    case InstructionType::MoveNorth:
        ship.position.y += instruction.value;
        return;
    case InstructionType::MoveSouth:
        ship.position.y -= instruction.value;
        return;
    case InstructionType::MoveEast:
        ship.position.x += instruction.value;
        return;
    case InstructionType::MoveWest:
        ship.position.x -= instruction.value;
        return;
    case InstructionType::RotateLeft:
        rotateLeft( ship, instruction.value );
        return;
    case InstructionType::RotateRight:
        rotateRight( ship, instruction.value );
        return;
    case InstructionType::MoveForward:
        moveForward( ship, instruction.value );
        return;
    }
    throw std::runtime_error( "invalid instruction" );
}

int getDistanceAfterInstructions( const std::vector<Instruction>& instructions )
{
    Ship ship;
    std::for_each( begin( instructions ), end( instructions ),
        [&ship] ( auto& instruction ) {
            executeInstruction( ship, instruction );
        } );
    return std::abs( ship.position.x ) + std::abs( ship.position.y );
}

void rotateWaypointLeft( ShipWithWaypoint& ship )
{
    std::swap( ship.waypoint.x, ship.waypoint.y );
    ship.waypoint.x = -ship.waypoint.x;
}

void rotateWaypointLeft( ShipWithWaypoint& ship, int value )
{
    while( value > 0 ) {
        rotateWaypointLeft( ship );
        value -= 90;
    }
}

void rotateWaypointRight( ShipWithWaypoint& ship )
{
    std::swap( ship.waypoint.x, ship.waypoint.y );
    ship.waypoint.y = -ship.waypoint.y;
}

void rotateWaypointRight( ShipWithWaypoint& ship, int value )
{
    while( value > 0 ) {
        rotateWaypointRight( ship );
        value -= 90;
    }
}

void moveForward( ShipWithWaypoint& ship, int value )
{
    ship.position.x += ship.waypoint.x * value;
    ship.position.y += ship.waypoint.y * value;
}

void executeInstruction( ShipWithWaypoint& ship, const Instruction& instruction )
{
    switch( instruction.instruction ) {
    case InstructionType::MoveNorth:
        ship.waypoint.y += instruction.value;
        return;
    case InstructionType::MoveSouth:
        ship.waypoint.y -= instruction.value;
        return;
    case InstructionType::MoveEast:
        ship.waypoint.x += instruction.value;
        return;
    case InstructionType::MoveWest:
        ship.waypoint.x -= instruction.value;
        return;
    case InstructionType::RotateLeft:
        rotateWaypointLeft( ship, instruction.value );
        return;
    case InstructionType::RotateRight:
        rotateWaypointRight( ship, instruction.value );
        return;
    case InstructionType::MoveForward:
        moveForward( ship, instruction.value );
        return;
    }
    throw std::runtime_error( "invalid instruction" );
}

int getDistanceAfterWaypointInstructions( const std::vector<Instruction>& instructions )
{
    ShipWithWaypoint ship;
    std::for_each( begin( instructions ), end( instructions ),
        [&ship] ( auto& instruction ) {
            executeInstruction( ship, instruction );
        } );
    return std::abs( ship.position.x ) + std::abs( ship.position.y );
}

void Day12()
{
    auto instructions = parseInstructions( "Day12Input.txt" );

    std::cout << "Day12:\n";
    std::cout << "Part1: Manhattan Distance from start: " << getDistanceAfterInstructions( instructions ) << "\n";
    std::cout << "Part2: Manhattan Distance from start with waypoint: " << getDistanceAfterWaypointInstructions( instructions ) << "\n";
}
