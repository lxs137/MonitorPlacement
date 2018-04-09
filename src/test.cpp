/* -*-c++-*- citygml2vrml - Copyright (c) 2010 Joachim Pouderoux, BRGM
*
* This file is part of libcitygml library
* http://code.google.com/p/libcitygml
*
* libcitygml is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 2.1 of the License, or
* (at your option) any later version.
*
* libcitygml is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*/

#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/cityobject.h>
#include "print.h"
#include "parser.h"

void analyzeObject( const citygml::CityObject*, unsigned int );

void usage()
{
    std::cout << "Usage: citygmltest [-options...] <filename>" << std::endl;
    std::cout << " Options:" << std::endl;
    std::cout << "  -log            Print some information during parsing" << std::endl;
    std::cout << "  -filter <mask>  CityGML objects to parse (default:All)" << std::endl
        << "                  The mask is composed of:" << std::endl
        << "                   GenericCityObject, Building, Room," << std::endl
        << "                   BuildingInstallation, BuildingFurniture, Door, Window, " << std::endl
        << "                   CityFurniture, Track, Road, Railway, Square, PlantCover," << std::endl
        << "                   SolitaryVegetationObject, WaterBody, TINRelief, LandUse," << std::endl
        << "                   Tunnel, Bridge, BridgeConstructionElement," << std::endl
        << "                   BridgeInstallation, BridgePart, All" << std::endl
        << "                  and seperators |,&,~." << std::endl
        << "                  Examples:" << std::endl
        << "                  \"All&~Track&~Room\" to parse everything but tracks & rooms" << std::endl
        << "                  \"Road&Railway\" to parse only roads & railways" << std::endl;
    std::cout << "  -destSRS <srs> Destination SRS (default: no transform)" << std::endl;
    exit( EXIT_FAILURE );
}

int main( int argc, char **argv )
{
    if ( argc < 2 ) usage();

    int fargc = 1;

    bool log = false;

    citygml::ParserParams params;

    for ( int i = 1; i < argc; i++ )
    {
        std::string param = std::string( argv[i] );
        std::transform( param.begin(), param.end(), param.begin(), tolower );
        if ( param == "-log" ) { log = true; fargc = i+1; }
        //if ( param == "-filter" ) { if ( i == argc - 1 ) usage(); params.objectsMask = argv[i+1]; i++; fargc = i+1; }
        if ( param == "-destsrs" ) { if ( i == argc - 1 ) usage(); params.destSRS = argv[i+1]; i++; fargc = i+1; }
    }

    if ( argc - fargc < 1 ) usage();

    std::cout << "Parsing CityGML file " << argv[fargc] << " using libcitygml v." << LIBCITYGML_VERSIONSTR << "..." << std::endl;

    time_t start;
    time( &start );

#if 0
    std::ifstream file;
    file.open( argv[fargc], std::ifstream::in );
     std::shared_ptr<const citygml::CityModel> city = citygml::load( file, params );
#else

    std::shared_ptr<const citygml::CityModel> city;
    try{
        city = citygml::load( argv[fargc], params );
    }catch(const std::runtime_error& e){
          
    }
#endif

    time_t end;
    time( &end );

    if ( !city ) return EXIT_FAILURE;

    std::cout << "Done in " << difftime( end, start ) << " seconds." << std::endl;

    if ( log )
    {
        std::cout << std::endl << "Objects hierarchy:" << std::endl;
        const citygml::ConstCityObjects& roots = city->getRootCityObjects();
        std::cout << "Envelope: " << city->getEnvelope() << std::endl;
        // std::vector<const citygml::CityObject*> parser_result = city->getAllCityObjectsOfType(citygml::CityObject::CityObjectsType::COT_Window);
        // for(auto it = parser_result.begin(); it != parser_result.end(); it++) {
        //     printCityObject(**it);
        // }
    }

    std::cout << "Done." << std::endl;

    return EXIT_SUCCESS;
}

void analyzeObject( const citygml::CityObject* object, unsigned int indent )
{
    if(object->getType() == citygml::CityObject::CityObjectsType::COT_Window) {
        printCityObject(*object);
    }

   for (unsigned int i = 0; i < object->getChildCityObjectsCount(); i++)
       analyzeObject(&(object->getChildCityObject(i)), indent+1);
}
