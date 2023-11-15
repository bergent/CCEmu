#include "client.h"
#include "server.h"
#include <iostream>
#include <string_view>

int main(int argc, char** argv) {
    
    // Usage handler
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " " << "<server/client>" << '\n';
        return 1;
    }

    std::string_view run_mode {argv[1]};

    if (run_mode == "server") {
        Server server {};
        server.Run(); 
    } 
    else if (run_mode == "client") {
        Client client {};
        client.Run(); 
    }
    else {
        std::cerr << "Usage: " << argv[0] << " " << "<server/client>" << '\n';
        return 1;
    }
    
    return 0;
}
