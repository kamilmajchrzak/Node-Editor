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
    Vector2 position;
    std::string label;
    std::vector<Port> inputs;
    std::vector<Port> outputs;

    Color color = { 50, 120, 180, 255 };
    Color content_color;
    int id;

    Node(int id_, const std::string& name, int numInputs, int numOutputs)
        : position{ (float)(200 + id_ * 150), 200 }, label(name), id(id_) {
        // Losuj: które porty są aktywne
        for (int i = 0; i < numInputs; ++i) {
            Port p;
            p.active = (rand() % 3 == 0); // ~33% szansy
            inputs.push_back(p);
        }
        for (int i = 0; i < numOutputs; ++i) {
            Port p;
            p.active = (rand() % 3 == 0);
            outputs.push_back(p);
        }

        content_color = {(unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), 255};

    }

    void UpdateHitboxes() {
        const int portSpacing = 20;
        const int portSize = 10;
        const int nodeWidth = 140;
        const int nodeHeight = 60 + std::max(inputs.size(), outputs.size()) * portSpacing;

        // Input ports (lewa strona)
        for (size_t i = 0; i < inputs.size(); ++i) {
            float y = position.y + 40 + i * portSpacing;
            inputs[i].hitbox = { position.x - portSize, y - portSize/2.0f, (float)portSize, (float)portSize };
        }
        // Output ports (prawa strona)
        for (size_t i = 0; i < outputs.size(); ++i) {
            float y = position.y + 40 + i * portSpacing;
            outputs[i].hitbox = { position.x + nodeWidth, y - portSize/2.0f, (float)portSize, (float)portSize };
        }
    }

    void Draw() const {
        const int nodeWidth = 140;
        const int nodeHeight = 60 + std::max(inputs.size(), outputs.size()) * 20;
        DrawRectangleRounded({ position.x, position.y, (float)nodeWidth, (float)nodeHeight }, 0.2f, 10, color);
        DrawRectangleRoundedLines({ position.x, position.y, (float)nodeWidth, (float)nodeHeight }, 0.2f, 20, WHITE);

        //DrawText(label.c_str(), (int)position.x + 10, (int)position.y + 15, 16, WHITE); // Replace with std::function, so to draw whatever digit/text/vector/bitmap/

        DrawCircle(this->position.x + (nodeWidth/2), this->position.y + (nodeHeight/2), 40, content_color);
        const int portSpacing = 20;
        const int portSize = 10;

        // Rysuj inputy (lewa strona)
        for (size_t i = 0; i < inputs.size(); ++i) {
            float y = position.y + 40 + i * portSpacing;
            Color c = inputs[i].active ? GREEN : DARKGRAY;
            DrawCircle(position.x, y, portSize/2.0f, c);
        }

        // Rysuj outputy (prawa strona)
        for (size_t i = 0; i < outputs.size(); ++i) {
            float y = position.y + 40 + i * portSpacing;
            Color c = outputs[i].active ? GREEN : DARKGRAY;
            DrawCircle(position.x + nodeWidth, y, portSize/2.0f, c);
        }
    }
};

struct Connection {
    int fromNode, fromPort;  // output node & index
    int toNode, toPort;      // input node & index
};

// Pomocnicza: pozycja portu (output)
Vector2 GetOutputPos(const Node& node, size_t portIdx) {
    const int nodeWidth = 140;
    const int portSpacing = 20;
    float y = node.position.y + 40 + portIdx * portSpacing;
    return { node.position.x + nodeWidth, y };
}

// Pomocnicza: pozycja portu (input)
Vector2 GetInputPos(const Node& node, size_t portIdx) {
    const int portSpacing = 20;
    float y = node.position.y + 40 + portIdx * portSpacing;
    return { node.position.x, y };
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 700, "Simple Node Editor (Raylib)");
    SetTargetFPS(60);

    srand((unsigned)time(nullptr));

    // Losowe nazwy
    std::vector<std::string> names = {"Start", "Filter", "Map", "Reduce", "Output"};
    std::vector<Node> nodes;
    for (int i = 0; i < 5; ++i) {
        int nIn = 1 + rand() % 4;
        int nOut = 1 + rand() % 4;
        nodes.emplace_back(i, names[i], nIn, nOut);
    }

    // Generuj przykładowe połączenia *tylko* między aktywnymi portami
    std::vector<Connection> connections;
    for (int tries = 0; tries < 8; ++tries) {
        Node& from = nodes[rand() % nodes.size()];
        Node& to   = nodes[rand() % nodes.size()];
        if (&from == &to) continue;

        // Szukaj aktywnego output w `from`
        std::vector<int> outActs, inActs;
        for (size_t i = 0; i < from.outputs.size(); ++i)
            if (from.outputs[i].active) outActs.push_back(i);
        for (size_t i = 0; i < to.inputs.size(); ++i)
            if (to.inputs[i].active) inActs.push_back(i);

        if (outActs.empty() || inActs.empty()) continue;

        Connection conn{
            from.id, outActs[rand() % outActs.size()],
            to.id,   inActs[rand() % inActs.size()]
        };

        // Unikaj duplikatów
        if (std::find_if(connections.begin(), connections.end(),
            [&](const Connection& c) {
                return c.fromNode == conn.fromNode && c.fromPort == conn.fromPort &&
                       c.toNode   == conn.toNode   && c.toPort   == conn.toPort;
            }) == connections.end()) {
            connections.push_back(conn);
        }
    }

    Node* draggedNode = nullptr;

    while (!WindowShouldClose()) {
        // Aktualizuj hitboxy portów (pozycje się zmieniają przy dragowaniu)
        for (auto& node : nodes) node.UpdateHitboxes();

        // Drag & drop
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (!draggedNode) {
                // Szukaj klikniętego węzła (bounding box)
                for (auto& node : nodes) {
                    Rectangle bb = { node.position.x, node.position.y, 140.0f,
                                     60.0f + std::max(node.inputs.size(), node.outputs.size()) * 20.0f };
                    if (CheckCollisionPointRec(mouse, bb)) {
                        draggedNode = &node;
                        break;
                    }
                }
            }
            if (draggedNode) {
                draggedNode->position = { mouse.x - 70, mouse.y - 30 }; // centruj pod myszą
            }
        } else {
            draggedNode = nullptr;
        }

        // Rysowanie
        BeginDrawing();
        ClearBackground({20, 25, 30, 255});

        // Rysuj połączenia *pod* węzłami (żeby nie przykrywały portów)
        for (const auto& conn : connections) {
            auto itFrom = std::find_if(nodes.begin(), nodes.end(), [&](const Node& n){ return n.id == conn.fromNode; });
            auto itTo   = std::find_if(nodes.begin(), nodes.end(), [&](const Node& n){ return n.id == conn.toNode; });
            if (itFrom == nodes.end() || itTo == nodes.end()) continue;

            Vector2 start = GetOutputPos(*itFrom, conn.fromPort);
            Vector2 end   = GetInputPos(*itTo,   conn.toPort);

            // Krzywa Béziera — lekko zakrzywiona
            Vector2 cp1 = { start.x + 80, start.y };
            Vector2 cp2 = { end.x   - 80, end.y };
            // Raylib nie ma DrawCubicBezier, więc użyjemy przybliżenia przez DrawLineBezier (liniowy Bézier) lub ręcznie...
            // Alternatywa: rysujemy "pseudo-bezier" jako dwa segmenty lub używamy własnej funkcji — tu dla prostoty:
            DrawLineBezier(start, end, 5, Fade(ORANGE, 0.7f));
        }

        // Rysuj węzły
        for (auto& node : nodes) {
            node.Draw();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}