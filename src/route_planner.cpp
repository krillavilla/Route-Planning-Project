#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // Find the closest nodes to the starting and ending coordinates.
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}

// Calculate h value using the distance to the end_node.
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

// Expand the current node by adding all unvisited neighbors to the open list.
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // Populate current_node.neighbors vector with all valid neighbors.
    current_node->FindNeighbors();

    for (auto neighbor : current_node->neighbors) {
        // Only process unvisited neighbors
        if (!neighbor->visited) {
            // Set parent, calculate g_value and h_value
            neighbor->parent = current_node;
            neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
            neighbor->h_value = CalculateHValue(neighbor);

            // Add to open_list and mark as visited
            neighbor->visited = true;
            open_list.push_back(neighbor);
        }
    }
}

// Sort the open list and return the next node.
RouteModel::Node *RoutePlanner::NextNode() {
    // Sort open_list according to the sum of h value and g value
    std::sort(open_list.begin(), open_list.end(), [](const auto &a, const auto &b) {
        return (a->g_value + a->h_value) > (b->g_value + b->h_value);
    });

    // Get the node with lowest sum
    RouteModel::Node *lowest_node = open_list.back();
    open_list.pop_back();

    return lowest_node;
}

// Construct the final path found from start_node to end_node.
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // Start from current_node and follow parent pointers back to start_node
    RouteModel::Node *p = current_node;

    while (p != nullptr) {
        path_found.push_back(*p);

        if (p->parent != nullptr) {
            // Calculate the distance between each node and its parent
            distance += p->distance(*(p->parent));
        }

        // Move to the parent node
        p = p->parent;
    }

    // Reverse the path since we built it backwards
    std::reverse(path_found.begin(), path_found.end());

    // Multiply the distance by the scale to get real-world distance
    distance *= m_Model.MetricScale();

    return path_found;
}

// A* Search algorithm
void RoutePlanner::AStarSearch() {
    // Initialize the start node
    start_node->visited = true;
    start_node->g_value = 0.0f;
    start_node->h_value = CalculateHValue(start_node);
    open_list.push_back(start_node);

    // Clear any previous path
    path_found.clear();

    // Main search loop
    while (!open_list.empty()) {
        // Get the next node with the lowest f-value (g + h)
        RouteModel::Node *current_node = NextNode();

        // Check if we've reached the end node
        if (current_node->distance(*end_node) == 0) {
            // We've found the destination, build the path
            m_Model.path = ConstructFinalPath(current_node);

            // Store the direct pointers to the nodes in the path
            path_found.clear();
            RouteModel::Node *p = current_node;

            // Work backwards from end node to start node, following parent pointers
            while (p != nullptr) {
                // Insert at beginning since we're working backwards
                path_found.insert(path_found.begin(), p);
                p = p->parent;
            }

            return;
        }

        // If we're not at the end node, add neighbors to the open list
        AddNeighbors(current_node);
    }

    // If we get here, no path was found
    path_found.clear();
    m_Model.path.clear();
}

// Implementation of GetPath method that matches the declaration in the header
std::vector<RouteModel::Node*> RoutePlanner::GetPath() const {
    return path_found;
}