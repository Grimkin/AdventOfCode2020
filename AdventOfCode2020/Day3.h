#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>


struct TreeMap
{
    int sizeX = 0;
    int sizeY = 0;
    std::vector<std::string> data;
};

TreeMap readTreeMap( std::string_view path )
{
    TreeMap treemap;
    std::ifstream file( path.data() );
    for( std::string line; std::getline( file, line ); ) {
        treemap.data.push_back( line );
        treemap.sizeY++;
    }
    treemap.sizeX = treemap.data[0].size();
    return treemap;
}

bool checkTreemap( int x, int y, const TreeMap& treeMap )
{
    return treeMap.data[y][x % treeMap.sizeX] == '#';
}

int traverseTreemap( int dirX, int dirY, const TreeMap& treeMap )
{
    int numTrees = 0;
    for( int y = 0, x = 0; y < treeMap.sizeY; y+=dirY,x+=dirX ) {
        numTrees += checkTreemap( x, y, treeMap );
    }
    return numTrees;
}

size_t sumDir( const std::vector<std::pair<int, int>>& directions, const TreeMap& treeMap )
{
    size_t numTrees = 1;
    for( auto& [dirX,dirY] : directions ) {
        numTrees *= traverseTreemap( dirX, dirY, treeMap );
    }
    return numTrees;
}

void Day3()
{
    auto treeMap = readTreeMap( "Day3Input.txt" );
    std::cout << "Day3:\n";
    std::cout << "Part1: Number of hit trees: " << traverseTreemap( 3, 1, treeMap ) << "\n";
    std::cout << "Part2: Number of hit trees: " << sumDir( { {1,1},{3,1},{5,1},{7,1},{1,2} }, treeMap ) << "\n";
}

