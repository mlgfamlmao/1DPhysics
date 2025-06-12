#include "bits.h"
#include "raylib.h"
using namespace std;



const int ScreenWidth = 2000;
const int ScreenHeight = 800;
const int TargetFPS = 240;

struct MassAndVelocity {
    int mass;
    float velocity;
    float coeffrestitution;
};

struct SimulationParams {
    float velocity1;
    float velocity2;
    int mass1;
    int mass2;
    float coeff1;
    float coeff2;
    bool isElastic;
    bool simulationStarted;
    bool parametersSet;
};

void RunSimulation(SimulationParams& params);
void Elastic(Rectangle square1, Rectangle square2, float& sq1, float& sq2, MassAndVelocity& square1obj, MassAndVelocity& square2obj, float& xsq1, float& xsq2);
void Inelastic(Rectangle square1, Rectangle square2, float& sq1, float& sq2, MassAndVelocity& square1obj, MassAndVelocity& square2obj, float& xsq1, float& xsq2);

bool IsCollisionX(Rectangle& square1, Rectangle& square2);
int IsCollisionWallX(Rectangle& square1, Rectangle& square2);
void DrawGUI(SimulationParams& params);
void HandleGUIInput(SimulationParams& params);

int main() {
    InitWindow(ScreenWidth, ScreenHeight, "Collision Simulator - Setup");
    SetTargetFPS(60);

    SimulationParams params = { 0 };
    params.mass1 = 10;
    params.mass2 = 10;
    params.velocity1 = 5.0f;
    params.velocity2 = 5.0f;
    params.coeff1 = 0.8f;
    params.coeff2 = 0.8f;
    params.isElastic = true;
    params.simulationStarted = false;
    params.parametersSet = false;

    while (!WindowShouldClose() && !params.simulationStarted) {
        HandleGUIInput(params);

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawGUI(params);
        EndDrawing();
    }

    if (params.simulationStarted) {
        RunSimulation(params);
    }

    CloseWindow();
    return 0;
}

void DrawGUI(SimulationParams& params) {
    // Title
    DrawText("COLLISION SIMULATOR", 50, 50, 60, RAYWHITE);
    DrawText("Setup Parameters", 50, 120, 40, LIGHTGRAY);

    // Collision Type Selection
    DrawText("Collision Type:", 50, 200, 30, RAYWHITE);

    Color elasticColor = params.isElastic ? GREEN : GRAY;
    Color inelasticColor = !params.isElastic ? GREEN : GRAY;

    DrawRectangle(50, 240, 200, 50, elasticColor);
    DrawText("ELASTIC", 90, 255, 20, BLACK);

    DrawRectangle(270, 240, 200, 50, inelasticColor);
    DrawText("INELASTIC", 300, 255, 20, BLACK);

    // Parameters
    DrawText("Square 1 Parameters:", 50, 320, 25, RAYWHITE);
    DrawText(("Mass: " + to_string(params.mass1) + " kg").c_str(), 50, 350, 20, LIGHTGRAY);
    DrawText(("Velocity: " + to_string((int)params.velocity1) + " m/s").c_str(), 50, 375, 20, LIGHTGRAY);

    DrawText("Square 2 Parameters:", 350, 320, 25, RAYWHITE);
    DrawText(("Mass: " + to_string(params.mass2) + " kg").c_str(), 350, 350, 20, LIGHTGRAY);
    DrawText(("Velocity: " + to_string((int)params.velocity2) + " m/s").c_str(), 350, 375, 20, LIGHTGRAY);

    if (!params.isElastic) {
        DrawText("Coefficient of Restitution:", 50, 420, 25, RAYWHITE);
        DrawText(("Square 1: " + to_string(params.coeff1).substr(0, 4)).c_str(), 50, 450, 20, LIGHTGRAY);
        DrawText(("Square 2: " + to_string(params.coeff2).substr(0, 4)).c_str(), 350, 450, 20, LIGHTGRAY);
    }

    // Controls
    DrawText("Controls:", 50, 520, 30, RAYWHITE);
    DrawText("Q/A - Adjust Square 1 Mass", 50, 560, 20, YELLOW);
    DrawText("W/S - Adjust Square 1 Velocity", 50, 585, 20, YELLOW);
    DrawText("E/D - Adjust Square 2 Mass", 50, 610, 20, YELLOW);
    DrawText("R/F - Adjust Square 2 Velocity", 50, 635, 20, YELLOW);

    if (!params.isElastic) {
        DrawText("T/G - Adjust Square 1 Coeff", 350, 560, 20, YELLOW);
        DrawText("Y/H - Adjust Square 2 Coeff", 350, 585, 20, YELLOW);
    }

    DrawText("1 - Select Elastic", 350, 610, 20, YELLOW);
    DrawText("2 - Select Inelastic", 350, 635, 20, YELLOW);

    // Start button
    DrawRectangle(50, 700, 200, 50, GREEN);
    DrawText("START SIMULATION", 70, 715, 20, BLACK);

    DrawText("SPACE - Start Simulation", 300, 725, 25, RAYWHITE);
}

void HandleGUIInput(SimulationParams& params) {
    // Collision type selection
    if (IsKeyPressed(KEY_ONE)) params.isElastic = true;
    if (IsKeyPressed(KEY_TWO)) params.isElastic = false;

    // Mouse clicks for collision type
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mousePos.x >= 50 && mousePos.x <= 250 && mousePos.y >= 240 && mousePos.y <= 290) {
            params.isElastic = true;
        }
        if (mousePos.x >= 270 && mousePos.x <= 470 && mousePos.y >= 240 && mousePos.y <= 290) {
            params.isElastic = false;
        }
        if (mousePos.x >= 50 && mousePos.x <= 250 && mousePos.y >= 700 && mousePos.y <= 750) {
            params.simulationStarted = true;
        }
    }

    // Parameter adjustments
    if (IsKeyPressed(KEY_Q)) params.mass1 = max(1, params.mass1 - 1);
    if (IsKeyPressed(KEY_A)) params.mass1 = min(100, params.mass1 + 1);

    if (IsKeyPressed(KEY_W)) params.velocity1 = max(0.0f, params.velocity1 - 1.0f);
    if (IsKeyPressed(KEY_S)) params.velocity1 = min(200.0f, params.velocity1 + 1.0f);

    if (IsKeyPressed(KEY_E)) params.mass2 = max(1, params.mass2 - 1);
    if (IsKeyPressed(KEY_D)) params.mass2 = min(100, params.mass2 + 1);

    if (IsKeyPressed(KEY_R)) params.velocity2 = max(0.0f, params.velocity2 - 1.0f);
    if (IsKeyPressed(KEY_F)) params.velocity2 = min(200.0f, params.velocity2 + 1.0f);

    if (!params.isElastic) {
        if (IsKeyPressed(KEY_T)) params.coeff1 = max(0.0f, params.coeff1 - 0.1f);
        if (IsKeyPressed(KEY_G)) params.coeff1 = min(1.0f, params.coeff1 + 0.1f);

        if (IsKeyPressed(KEY_Y)) params.coeff2 = max(0.0f, params.coeff2 - 0.1f);
        if (IsKeyPressed(KEY_H)) params.coeff2 = min(1.0f, params.coeff2 + 0.1f);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        params.simulationStarted = true;
    }
}

void RunSimulation(SimulationParams& params) {
    CloseWindow();

    MassAndVelocity square1obj = { params.mass1, params.velocity1 * 10.0f, params.coeff1 };
    MassAndVelocity square2obj = { params.mass2, -params.velocity2 * 10.0f, params.coeff2 };

    float xsq1 = 100.0f;
    float xsq2 = 1800.0f;
    Rectangle square1 = { xsq1, 400, 70, 70 };
    Rectangle square2 = { xsq2, 400, 70, 70 };
    float sq1 = 0.1f;
    float sq2 = 1930.0f;

    if (params.isElastic) {
        Elastic(square1, square2, sq1, sq2, square1obj, square2obj, xsq1, xsq2);
    }
    else {
        Inelastic(square1, square2, sq1, sq2, square1obj, square2obj, xsq1, xsq2);
    }
}

void Elastic(Rectangle square1, Rectangle square2, float& sq1, float& sq2, MassAndVelocity& square1obj, MassAndVelocity& square2obj, float& xsq1, float& xsq2) {
    InitWindow(ScreenWidth, ScreenHeight, "Elastic Collision Simulator");
    SetTargetFPS(TargetFPS);

    while (!WindowShouldClose()) {
        const float FixedDeltaTime = 1.0f / 240.0f;

        if (IsCollisionX(square1, square2)) {
            float overlap = (square1.x + square1.width) - square2.x;
            if (overlap > 0) {
                xsq1 -= (overlap / 2.0f + 0.1f);
                xsq2 += (overlap / 2.0f + 0.1f);
                square1.x = xsq1;
                square2.x = xsq2;
            }

            float v1 = square1obj.velocity;
            float v2 = square2obj.velocity;

            // Elastic collision formulas
            square1obj.velocity = ((square1obj.mass - square2obj.mass) * v1 + 2 * square2obj.mass * v2) / (square1obj.mass + square2obj.mass);
            square2obj.velocity = ((square2obj.mass - square1obj.mass) * v2 + 2 * square1obj.mass * v1) / (square1obj.mass + square2obj.mass);
        }

        int wallCollision = IsCollisionWallX(square1, square2);
        if (wallCollision == 2) {
            square1obj.velocity *= -1;
        }
        else if (wallCollision == 3) {
            square2obj.velocity *= -1;
        }

        xsq1 += square1obj.velocity * FixedDeltaTime;
        xsq2 += square2obj.velocity * FixedDeltaTime;
        square1.x = xsq1;
        square2.x = xsq2;

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw title
        DrawText("ELASTIC COLLISION", 50, 50, 40, RAYWHITE);

        DrawRectangleRec(square1, RED);
        DrawRectangleRec(square2, BLUE);

        // Display information near squares
        string velocityStr1 = "V1: " + to_string((int)(square1obj.velocity / 10)) + "m/s";
        string velocityStr2 = "V2: " + to_string((int)(square2obj.velocity / 10)) + "m/s";
        string massStr1 = "M1: " + to_string(square1obj.mass) + "kg";
        string massStr2 = "M2: " + to_string(square2obj.mass) + "kg";

        DrawText(velocityStr1.c_str(), xsq1, square1.y + 80, 20, RAYWHITE);
        DrawText(massStr1.c_str(), xsq1, square1.y + 100, 20, RAYWHITE);

        DrawText(velocityStr2.c_str(), xsq2, square2.y + 80, 20, RAYWHITE);
        DrawText(massStr2.c_str(), xsq2, square2.y + 100, 20, RAYWHITE);

        // HUD information
        DrawText("Square 1 (Red):", 50, 650, 25, RED);
        DrawText(velocityStr1.c_str(), 50, 680, 20, RAYWHITE);
        DrawText(massStr1.c_str(), 50, 700, 20, RAYWHITE);

        DrawText("Square 2 (Blue):", 300, 650, 25, BLUE);
        DrawText(velocityStr2.c_str(), 300, 680, 20, RAYWHITE);
        DrawText(massStr2.c_str(), 300, 700, 20, RAYWHITE);

        // Calculate and display kinetic energy
        float ke1 = 0.5f * square1obj.mass * (square1obj.velocity / 10) * (square1obj.velocity / 10);
        float ke2 = 0.5f * square2obj.mass * (square2obj.velocity / 10) * (square2obj.velocity / 10);
        float totalKE = ke1 + ke2;

        DrawText(("Total KE: " + to_string((int)totalKE) + "J").c_str(), 600, 680, 20, YELLOW);
        DrawText("(Kinetic Energy Conserved)", 600, 700, 18, GREEN);

        EndDrawing();
    }
    CloseWindow();
}

void Inelastic(Rectangle square1, Rectangle square2, float& sq1, float& sq2, MassAndVelocity& square1obj, MassAndVelocity& square2obj, float& xsq1, float& xsq2) {
    InitWindow(ScreenWidth, ScreenHeight, "Inelastic Collision Simulator");
    SetTargetFPS(TargetFPS);

    while (!WindowShouldClose()) {
        const float FixedDeltaTime = 1.0f / 240.0f;

        if (IsCollisionX(square1, square2)) {
            float overlap = (square1.x + square1.width) - square2.x;
            if (overlap > 0) {
                xsq1 -= (overlap / 2.0f + 0.1f);
                xsq2 += (overlap / 2.0f + 0.1f);
                square1.x = xsq1;
                square2.x = xsq2;
            }

            float v1 = square1obj.velocity;
            float v2 = square2obj.velocity;
            float m1 = square1obj.mass;
            float m2 = square2obj.mass;
            float e = (square1obj.coeffrestitution + square2obj.coeffrestitution) / 2.0f; // Average coefficient

            // Inelastic collision formulas using coefficient of restitution
            float v1_final = (m1 * v1 + m2 * v2 + m2 * e * (v2 - v1)) / (m1 + m2);
            float v2_final = (m1 * v1 + m2 * v2 + m1 * e * (v1 - v2)) / (m1 + m2);

            square1obj.velocity = v1_final;
            square2obj.velocity = v2_final;
        }

        int wallCollision = IsCollisionWallX(square1, square2);
        if (wallCollision == 2) {
            square1obj.velocity *= -square1obj.coeffrestitution;
        }
        else if (wallCollision == 3) {
            square2obj.velocity *= -square2obj.coeffrestitution;
        }

        xsq1 += square1obj.velocity * FixedDeltaTime;
        xsq2 += square2obj.velocity * FixedDeltaTime;
        square1.x = xsq1;
        square2.x = xsq2;

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw title
        DrawText("INELASTIC COLLISION", 50, 50, 40, RAYWHITE);

        DrawRectangleRec(square1, RED);
        DrawRectangleRec(square2, BLUE);

        // Display information near squares
        string velocityStr1 = "V1: " + to_string((int)(square1obj.velocity / 10)) + "m/s";
        string velocityStr2 = "V2: " + to_string((int)(square2obj.velocity / 10)) + "m/s";
        string massStr1 = "M1: " + to_string(square1obj.mass) + "kg";
        string massStr2 = "M2: " + to_string(square2obj.mass) + "kg";

        DrawText(velocityStr1.c_str(), xsq1, square1.y + 80, 20, RAYWHITE);
        DrawText(massStr1.c_str(), xsq1, square1.y + 100, 20, RAYWHITE);

        DrawText(velocityStr2.c_str(), xsq2, square2.y + 80, 20, RAYWHITE);
        DrawText(massStr2.c_str(), xsq2, square2.y + 100, 20, RAYWHITE);

        // HUD information
        DrawText("Square 1 (Red):", 50, 600, 25, RED);
        DrawText(velocityStr1.c_str(), 50, 630, 20, RAYWHITE);
        DrawText(massStr1.c_str(), 50, 650, 20, RAYWHITE);
        DrawText(("Coeff: " + to_string(square1obj.coeffrestitution).substr(0, 4)).c_str(), 50, 670, 20, RAYWHITE);

        DrawText("Square 2 (Blue):", 300, 600, 25, BLUE);
        DrawText(velocityStr2.c_str(), 300, 630, 20, RAYWHITE);
        DrawText(massStr2.c_str(), 300, 650, 20, RAYWHITE);
        DrawText(("Coeff: " + to_string(square2obj.coeffrestitution).substr(0, 4)).c_str(), 300, 670, 20, RAYWHITE);

        // Calculate and display kinetic energy
        float ke1 = 0.5f * square1obj.mass * (square1obj.velocity / 10) * (square1obj.velocity / 10);
        float ke2 = 0.5f * square2obj.mass * (square2obj.velocity / 10) * (square2obj.velocity / 10);
        float totalKE = ke1 + ke2;

        DrawText(("Total KE: " + to_string((int)totalKE) + "J").c_str(), 600, 650, 20, YELLOW);
        DrawText("(Energy NOT Conserved)", 600, 670, 18, RED);

        EndDrawing();
    }
    CloseWindow();
}

bool IsCollisionX(Rectangle& square1, Rectangle& square2) {
    return square1.x < square2.x + square2.width &&
        square1.x + square1.width > square2.x;
}




int IsCollisionWallX(Rectangle& square1, Rectangle& square2) {
    if (square1.x <= 0 || square1.x + square1.width >= ScreenWidth) return 2;
    if (square2.x <= 0 || square2.x + square2.width >= ScreenWidth) return 3;
    return 0;
}



