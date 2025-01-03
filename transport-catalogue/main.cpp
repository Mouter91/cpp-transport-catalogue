#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "json_reader.h"
#include "transport_catalogue.h"

int main()
{
    TransportCatalogue cat;

    LoadJson json(std::cin, cat);
    json.GetReply();

    return 0;
}
