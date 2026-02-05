//
// Created by Kamil Majchrzak on 01/12/2025.
//

#ifndef _NODE_H_
#define _NODE_H_

#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <functional>

struct Port {
    bool active = false;  // czy port jest połączony (aktywny)
    Rectangle hitbox;
};

struct Node {
    int id;
    Vector2 position;
    std::string label;
    std::vector<Port> inputs;
    std::vector<Port> outputs;
    Color color = { 50, 120, 180, 255 };
    Color content_color{};
    int textWidth;
    int defaultFontSize;

    Node(int id_, const std::string& name, int numInputs, int numOutputs);

    void UpdateHitboxes();

    void Draw() const;
};

struct Connection {
    int fromNode, fromPort;  // output node & index
    int toNode, toPort;      // input node & index
};

Vector2 GetOutputPos(const Node& node, size_t portIdx);

Vector2 GetInputPos(const Node& node, size_t portIdx);



#endif //_NODE_H