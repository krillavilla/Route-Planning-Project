#include <iostream>
#include <string>
#include <optional>
#include <limits>
#include <fstream>
#include <vector>
#include "route_model.h"
#include "route_planner.h"
#include "render.h"

// Default map file path
static std::string default_map_path = "../map.osm";

// Function to parse command line arguments
std::string ParseArguments(int argc, char **argv) {
    std::string osm_file_path = default_map_path;

    // Process command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-f" && i + 1 < argc) {
            osm_file_path = argv[i + 1];
            i++; // Skip the next argument as we've processed it
        }
    }

    return osm_file_path;
}

// Function to validate user input is within range 0-100
bool ValidateInput(float value) {
    return value >= 0.0f && value <= 100.0f;
}

// Function to read OSM data from file
std::vector<std::byte> ReadOSMData(const std::string &path) {
    std::vector<std::byte> osm_data;

    // Open the file with the binary flag
    std::ifstream osm_file(path, std::ios::binary);
    if (!osm_file) {
        std::cerr << "Failed to open OSM file: " << path << std::endl;
        return osm_data;
    }

    // Find the size of the file
    osm_file.seekg(0, std::ios::end);
    const auto size = osm_file.tellg();
    osm_file.seekg(0, std::ios::beg);

    // Reserve space in the vector
    osm_data.resize(static_cast<size_t>(size));

    // Read the data
    osm_file.read(reinterpret_cast<char*>(osm_data.data()), size);

    return osm_data;
}

int main(int argc, char **argv) {
    // Parse command line arguments
    std::string osm_file_path = ParseArguments(argc, argv);

    // Print the file being used
    std::cout << "Reading OpenStreetMap data from: " << osm_file_path << std::endl;

    // Check if the file exists
    std::ifstream map_file(osm_file_path.c_str());
    if (!map_file) {
        std::cerr << "Error: Could not open the map file: " << osm_file_path << std::endl;
        return -1;
    }
    map_file.close();

    // Read OSM data
    std::vector<std::byte> osm_data = ReadOSMData(osm_file_path);
    if (osm_data.empty()) {
        std::cerr << "Error: Could not read OSM data." << std::endl;
        return -1;
    }

    // Create Model
    RouteModel model{osm_data};

    // Get user input for start and end coordinates with validation
    float start_x, start_y, end_x, end_y;
    bool valid_input = false;

    while (!valid_input) {
        std::cout << "Enter start x coordinate (0-100): ";
        std::cin >> start_x;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter numeric values.\n";
            continue;
        }

        std::cout << "Enter start y coordinate (0-100): ";
        std::cin >> start_y;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter numeric values.\n";
            continue;
        }

        std::cout << "Enter end x coordinate (0-100): ";
        std::cin >> end_x;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter numeric values.\n";
            continue;
        }

        std::cout << "Enter end y coordinate (0-100): ";
        std::cin >> end_y;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter numeric values.\n";
            continue;
        }

        // Validate that all coordinates are within range (0-100)
        if (ValidateInput(start_x) && ValidateInput(start_y) &&
            ValidateInput(end_x) && ValidateInput(end_y)) {
            valid_input = true;
        } else {
            std::cout << "Invalid input! Coordinates must be between 0-100.\n";
        }
    }

    // Create RoutePlanner and perform A* search
    RoutePlanner route_planner{model, start_x, start_y, end_x, end_y};
    route_planner.AStarSearch();

    // Get distance
    float distance = route_planner.GetDistance();
    std::cout << "Distance: " << distance << " meters" << std::endl;

    // Render the map and the route
    Render render{model};

    // Call Display without any parameters - assuming the Render class will take care of displaying
    // the route that we've calculated with the route_planner above
    // Replace the render.Display() line with:
    render.Display(route_planner.GetPath());

    return 0;
}