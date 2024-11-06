#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "json_reader.h"
#include "transport_catalogue.h"

int main()
{
    TransportCatalogue cat;
    
    std::stringstream buffer;
    buffer << std::cin.rdbuf(); 

    std::istringstream fileStream(buffer.str());

    LoadJson json(fileStream, cat);
    json.GetReply();

    return 0;
}
