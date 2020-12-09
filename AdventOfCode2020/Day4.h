#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <regex>

std::vector<std::string> getData( std::string_view path )
{
    std::vector<std::string> data;
    std::ifstream file( path.data() );
    std::string line;
    data.push_back( "" );
    for( std::string line; std::getline( file, line ); ) {
        if( line.empty() )
            data.push_back( "" );
        data.back() += " " + line;
    }
    return data;
}

bool checkFields( const std::string& passport, const std::vector<std::string>& fields )
{
    for( auto& field : fields ) {
        if( passport.find( field ) == std::string::npos )
            return false;
    }

    return true;
}

bool checkYear( const std::string& passport, const std::string& field, int minValue, int maxValue )
{
    const std::regex regex( field + R"(:((\d){4})\b)" );
    std::smatch match;
    if( !std::regex_search( passport, match, regex ) )
        return false;

    try {
        int year = std::stoi( match[1] );
        if( year < minValue || year > maxValue )
            return false;
    }
    catch( const std::exception& ) {
        return false;
    }
    return true;
}

bool checkBirthYear( const std::string& passport )
{
    return checkYear( passport, "byr", 1920, 2002 );
}

bool checkIssueYear( const std::string& passport )
{
    return checkYear( passport, "iyr", 2010, 2020 );
}

bool checkExpirationData( const std::string& passport )
{
    return checkYear( passport, "eyr", 2020, 2030 );
}

bool checkHeight( const std::string& passport )
{
    const std::regex regex( R"(hgt:(\d+)(\w\w)\b)" );
    std::smatch match;
    if( !std::regex_search( passport, match, regex ) )
        return false;

    int height = 0;
    try {
        height = std::stoi( match[1] );
    }
    catch( const std::exception& ) {
        return false;
    }

    if( match[2] == "in" )
        return height >= 59 && height <= 76;
    if( match[2] == "cm" )
        return height >= 150 && height <= 193;

    return false;
}

bool checkHairColor( const std::string& passport )
{
    const std::regex regex( R"(hcl:#(\d|[a-f]){6}\b)" );
    std::smatch match;
    return std::regex_search( passport, match, regex );
}

bool checkEyeColor( const std::string& passport )
{
    const std::regex regex( R"(ecl:((\w){3})\b)" );
    std::smatch match;
    if( !std::regex_search( passport, match, regex ) )
        return false;

    for( auto& color : {"amb", "blu", "brn", "gry","grn","hzl","oth"} ) {
        if( match[1] == color )
            return true;
    }

    return false;
}

bool checkPassportId( const std::string& passport )
{
    const std::regex regex( R"(pid:(\d){9}\b)" );
    std::smatch match;
    return std::regex_search( passport, match, regex );
}

int numPassportsWithAllFields( const std::vector<std::string>& passports, const std::vector<std::string>& fields )
{
    return std::accumulate( passports.begin(), passports.end(), 0, [&fields] ( int sum, const auto& passport ) { return sum + checkFields( passport, fields ); } );
}

bool checkPassport( const std::string& passport )
{
    return checkBirthYear( passport )
        && checkIssueYear( passport )
        && checkExpirationData( passport )
        && checkHeight( passport )
        && checkHairColor( passport )
        && checkEyeColor( passport )
        && checkPassportId( passport );
}

int numValidPassports( const std::vector<std::string>& passports )
{
    return std::accumulate( passports.begin(), passports.end(), 0,
        [] ( int sum, const auto& passport )
        {
            return sum + checkPassport( passport ); }
    );
}

void Day4()
{
    auto passports = getData( "Day4Input.txt" );

    std::cout << "Day4:\n";
    std::cout << "Part1: Number of valid passport: " << numPassportsWithAllFields( passports, { "byr","iyr","eyr","hgt","hcl","ecl","pid" } ) << "\n";
    std::cout << "Part2: Number of valid passport: " << numValidPassports( passports ) << "\n";
}
