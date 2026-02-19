// performs the computation and returns the results to the
// web page frontend using HTTP requests

#include <iostream>
#include <vector>
#include <algorithm>
#include "httplib.h"

struct Load {
    std::string name;
    int power;
    int priority; // lower number = higher priority
};

bool compareLoadsByPriority(const Load& first, const Load& second){
    return first.priority < second.priority;
}


int main() {
    // Create HTTP server
    httplib::Server server;

    // enables cross origin resource sharing
    server.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },
        { "Access-Control-Allow-Methods", "POST, GET, OPTIONS" },
        { "Access-Control-Allow-Headers", "Content-Type" }
    });

    // Handle preflight requests
    server.Options("/shed", [](const httplib::Request&, httplib::Response& res) {res.status = 200;});

    //Load shedding endpoint that receives request and sends response
    server.Post("/shed", [](const httplib::Request& request, httplib::Response& response) 
        {
            int capacity = std::stoi(request.get_param_value("capacity"));
            int count = std::stoi(request.get_param_value("count"));

            std::vector<Load> loads;

            // Read loads from request to use them in the program
            for (int i = 0; i < count; i++) {
                Load loadObject;
                loadObject.name = request.get_param_value(
                    "name" + std::to_string(i)
                );
                loadObject.power = std::stoi(
                    request.get_param_value("power" + std::to_string(i))
                );
                loadObject.priority = std::stoi(
                    request.get_param_value("priority" + std::to_string(i))
                );
                loads.push_back(loadObject);
            }

            // sorting loads by priority to make it easier to 
            // shed the ones with least priority
            std::sort(loads.begin(), loads.end(), compareLoadsByPriority);

            int used = 0;

            std::vector<std::string> served;
            std::vector<std::string> shed;

            // serves the loads according to their priority
            for (const Load& loadObject : loads) {
                if (used + loadObject.power <= capacity) {
                    used += loadObject.power;
                    served.push_back(loadObject.name);
                } else {
                    shed.push_back(loadObject.name);
                }
            }

            // Build JSON response to send to the frontend
            std::string jsonResponse = "{";

            jsonResponse += "\"served\": [";
            for (size_t i = 0; i < served.size(); i++) {
                jsonResponse += "\"" + served[i] + "\"";
                if (i < served.size() - 1) {
                    jsonResponse += ", ";
                }
            }
            jsonResponse += "],";

            jsonResponse += "\"shed\": [";
            for (size_t i = 0; i < shed.size(); i++) {
                jsonResponse += "\"" + shed[i] + "\"";
                if (i < shed.size() - 1) {
                    jsonResponse += ", ";
                }
            }
            jsonResponse += "],";

            jsonResponse += "\"used\": " + std::to_string(used);
            jsonResponse += "}";

            // Send response to the frontend
            response.set_content(jsonResponse, "application/json");
        }
    );

    std::cout << "Server running at http://localhost:8080\n";
    server.listen("0.0.0.0", 8080);
}
