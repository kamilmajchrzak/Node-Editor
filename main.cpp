#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <functional>

#include "Node.h"



//Electronic symbols - test?
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 700, "Electronic Symbols (Raylib)");
    SetTargetFPS(60);

    srand((unsigned)time(nullptr));

    // Losowe nazwy
    // std::vector<std::string> names = {"Diode", "Resistor", "Capacitor", "Inductor", "Op-amp"};
    // std::vector<Node> nodes;
    // for (int i = 0; i < 5; ++i) {
    //     int nIn = 1 + rand() % 4;
    //     int nOut = 1 + rand() % 4;
    //     nodes.emplace_back(i, names[i], nIn, nOut);
    // }

    Node diode(1,"diode",1,1);
    Node op_amp(2,"BC-547", 2, 1);
    Node ic(3, "uC - ARM Cortex", 8, 8);
    Connection connection(1,0,2,0);
    std::vector<Node> nodes;
    nodes.push_back(diode);
    nodes.push_back(op_amp);
    nodes.push_back(ic);

    std::vector<Connection> connections;
    connections.push_back(connection);
    // Generuj przykładowe połączenia *tylko* między aktywnymi portami
    // std::vector<Connection> connections;
    // for (int tries = 0; tries < 8; ++tries) {
    //     Node& from = nodes[rand() % nodes.size()];
    //     Node& to   = nodes[rand() % nodes.size()];
    //     if (&from == &to) continue;
    //
    //     // Szukaj aktywnego output w `from`
    //     std::vector<int> outActs, inActs;
    //     for (size_t i = 0; i < from.outputs.size(); ++i)
    //         if (from.outputs[i].active) outActs.push_back(i);
    //     for (size_t i = 0; i < to.inputs.size(); ++i)
    //         if (to.inputs[i].active) inActs.push_back(i);
    //
    //     if (outActs.empty() || inActs.empty()) continue;
    //
    //     Connection conn{
    //         from.id, outActs[rand() % outActs.size()],
    //         to.id,   inActs[rand() % inActs.size()]
    //     };
    //
    //     // Unikaj duplikatów
    //     if (std::find_if(connections.begin(), connections.end(),
    //         [&](const Connection& c) {
    //             return c.fromNode == conn.fromNode && c.fromPort == conn.fromPort &&
    //                    c.toNode   == conn.toNode   && c.toPort   == conn.toPort;
    //         }) == connections.end()) {
    //         connections.push_back(conn);
    //     }
    // }

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