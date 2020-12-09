#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>

struct Rule
{
    char letter = '\0';
    size_t minNum = 0;
    size_t maxNum = std::numeric_limits<size_t>::max();
};

struct Password
{
    std::string password;
    Rule rule;
};

std::vector<Password> createPasswords( const std::string& data )
{
    std::vector<Password> passwords;
    std::istringstream ss( data );
    std::string line;
    std::regex regex( R"((\d*)-(\d*) (\w): (\w*))" );
    for( std::string line; std::getline( ss, line ); ) {
        std::smatch match;
        if( !std::regex_match( line, match, regex ) )
            continue;
        size_t min = std::stoull( match[1] );
        size_t max = std::stoull( match[2] );
        char letter = match[3].str()[0];
        std::string password = match[4];
        passwords.push_back( { password,{letter,min,max} } );
    }
    return passwords;
}

bool isPasswordCorrectDay1( const Password& password )
{
    auto numLetters = std::accumulate( begin( password.password ), end( password.password ), 0,
        [&password] ( int sum, char letter ) {
            return sum + ( password.rule.letter == letter );
        } );
    return numLetters >= password.rule.minNum && numLetters <= password.rule.maxNum;
}

bool isPasswordCorrectDay2( const Password& password )
{
    auto& rule = password.rule;
    return ( ( password.password[rule.minNum - 1] == rule.letter ) + ( password.password[rule.maxNum - 1] == rule.letter ) ) == 1;
}

template<typename T>
size_t getNumCorrectPasswords( const std::vector<Password>& passwords, T isPasswordCorrect )
{
    return std::accumulate( begin( passwords ), end( passwords ), 0, [&isPasswordCorrect] ( int sum, const auto& password ) { return sum + isPasswordCorrect( password ); } );
}

void Day2()
{
    std::ifstream file( "Day2Input.txt" );
    std::string day2Input( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
    auto passwords = createPasswords( day2Input );
    std::cout << "Day2:\n";
    std::cout << "Part1: Number of correct passwords: " << getNumCorrectPasswords( passwords, &isPasswordCorrectDay1 ) << "\n";
    std::cout << "Part2: Number of correct passwords: " << getNumCorrectPasswords( passwords, &isPasswordCorrectDay2 ) << "\n";
}
