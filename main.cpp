#define OLC_PGE_APPLICATION
#define OLC_PGEX_FONT
#include "olcPixelGameEngine.h"
#include "types3d.h"
#include <chrono>
#include "sim.h"


using namespace olc;
using namespace std;

class Main : public PixelGameEngine
{
    private:
        int mouseX, mouseY;
        int prevMouseX, prevMouseY;
        int screenW, screenH;
        bool frozen = true;
        bool quickAdd = false;
        bool isRemoving = false;
        bool isMoving = false;
        vec2d moveOffset = vec2d();
        point* firstStickPoint = nullptr;
        point* prevQuickAddedPoint = nullptr;
        chrono::steady_clock::time_point timeLastQuickAdd = chrono::high_resolution_clock::now();
        float closestPointDistLimit = 10.0f;
        vec2d origin;
        ClothSim cs;

        const string helpText =
            (string)
            "=== SPLINTER - 2D Physics Simulator ===\n\n\n"
            "Controls:\n\n" +
            "  [LMB]                 - Add point\n\n" +
            "  [Shift] + [LMB]       - Remove point\n\n" +
            "  [RMB]                 - Create line\n\n" +
            "  [Ctrl] + [LMB]        - Move camera\n\n" +
            "  [MMB]                 - Lock point\n\n" +
            "  [Q] + [LMB]           - Quick add points + lines\n\n" +
            "  [Shift] + [Q] + [LMB] - Quick remove points\n\n" +
            "  [C]                   - Clear all points + lines\n\n" +
            "  [Space]               - Play/Pause simulation";

    public:
        Main()
        {
            sAppName = "Splinter";
            bShowFPS = true;
        }
    
    public:
    bool OnUserCreate() override
    {
        screenW = ScreenWidth();
        screenH = ScreenHeight();

        cs = ClothSim(10);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        mouseX = max(0, min(ScreenWidth(), GetMouseX()));
        mouseY = max(0, min(ScreenHeight(), GetMouseY()));
        vec2d mousePos = vec2d(mouseX, mouseY);

        auto mod = [](float a, float b)
        {
            return a - b * floor(a / b);
        };

        Clear(BLACK);
        //Grid
        const int numGridLines = 5;
        for (int i = 0; i < numGridLines; i++)
        {
            float lineOffsetX = -moveOffset.x + i * (screenW / numGridLines);
            float lineOffsetY = -moveOffset.y + i * (screenH / numGridLines);

			DrawLine(mod(lineOffsetX, screenW), 0, mod(lineOffsetX, screenW), screenH, WHITE * 0.1);
			DrawLine(0, mod(lineOffsetY, screenH), screenW, mod(lineOffsetY, screenH), WHITE * 0.1);
        }
        // Mouse circle
        DrawCircle(mousePos.asVf(), closestPointDistLimit, WHITE * 0.15);

        isRemoving = GetKey(Key::SHIFT).bHeld;

        //INPUT
        if (GetKey(Key::CTRL).bHeld) //Moving
        {
            if (GetMouse(0).bHeld)
            {
                moveOffset += vec2d(prevMouseX, prevMouseY) - mousePos;
            }
			isMoving = true;
        }
        else //Not moving
        {
            isMoving = false;
            quickAdd = GetKey(Key::Q).bHeld;

            if (GetKey(Key::SPACE).bPressed) //Toggle freeze
            {
                frozen = !frozen;
            }
            if (GetKey(Key::C).bPressed) //Clear
            {
                cs.Clear();
                frozen = true;
            }

            if (GetKey(Key::Q).bHeld) //Quick add
            {
                if (GetMouse(0).bHeld)
                {
                    auto currTime = chrono::high_resolution_clock::now();

                    if (isRemoving)
                    {
                        cs.RemoveNearestPoint(mousePos + moveOffset, closestPointDistLimit);
                    }
                    else if(chrono::duration_cast<chrono::milliseconds>(currTime - timeLastQuickAdd).count() > 20)
                    {
						point* newQuickAddedPoint = cs.AddPoint(mouseX + moveOffset.x, mouseY + moveOffset.y);
                        if (prevQuickAddedPoint != nullptr)
                        {
							cs.AddStick(prevQuickAddedPoint, newQuickAddedPoint);
                        }
                        prevQuickAddedPoint = newQuickAddedPoint;
                        timeLastQuickAdd = currTime;
                    }
                }
                else
                {
                    prevQuickAddedPoint = nullptr;
                }
            }
            else //Normal add
            {
                if (GetMouse(0).bPressed && firstStickPoint == nullptr) //Add/Remove point
                {
                    if (isRemoving)
                    {
						cs.RemoveNearestPoint(mousePos + moveOffset, closestPointDistLimit);
                    }
                    else
                    {
						cs.AddPoint(mouseX + moveOffset.x, mouseY + moveOffset.y);
                    }
                }
                if (GetMouse(1).bPressed) //Add stick
                {
                    if (firstStickPoint == nullptr)
                    {
                        firstStickPoint = cs.GetClosestPoint(mousePos + moveOffset, closestPointDistLimit);
                    }
                    else
                    {
                        point* secondStickPoint = cs.GetClosestPoint(mousePos + moveOffset, closestPointDistLimit);
                        if (secondStickPoint != nullptr)
                        {
                            cs.AddStick(firstStickPoint, secondStickPoint);
                        }
                        firstStickPoint = nullptr;
                    }
                }
                if (GetMouse(2).bPressed) //Lock point
                {
                    point* selectedPoint = cs.GetClosestPoint(vec2d(mouseX, mouseY) + moveOffset, closestPointDistLimit);
                    if (selectedPoint != nullptr)
                    {
                        selectedPoint->locked = !selectedPoint->locked;
                    }
                }
            }
        }

        if (!frozen)
        {
            cs.Simulate(fElapsedTime);
        }
        cs.Draw(this, moveOffset);

        //Closest point
        point* closestPt = cs.GetClosestPoint(mousePos + moveOffset, closestPointDistLimit);
        if (closestPt != nullptr)
        {
            DrawLine((closestPt->pos + vec2d(-3,  5) - moveOffset).asVf(), (closestPt->pos + vec2d(3,  5) - moveOffset).asVf(), BLUE);
            DrawLine((closestPt->pos + vec2d(-3, -5) - moveOffset).asVf(), (closestPt->pos + vec2d(3, -5) - moveOffset).asVf(), BLUE);
        }
        // Half-drawn stick
        if (firstStickPoint != nullptr)
        {
            DrawLine((firstStickPoint->pos - moveOffset).asVf(), vi2d(mouseX, mouseY), WHITE * 0.4);
        }
        //Info
        DrawString(2, screenH - 24, (string)(isRemoving ? (quickAdd ? "QUICK REMOVE" : "REMOVE"):(quickAdd ? "QUICK ADD" : "ADD")) + "   " + (isMoving ? "MOVE" : ""), WHITE * 0.8);
        DrawString(2, screenH - 12, (frozen ? "FROZEN" : "SIMULATING") + ("   Points: " + to_string(cs.GetNumPoints())) + ("   Lines: " + to_string(cs.GetNumSticks())) + "   Press [H] for help.", WHITE * 0.8);

        if (GetKey(Key::H).bHeld)
        {
            DrawString(30, screenW / 2 - 110, helpText, GREEN*0.3 + WHITE*0.4);
        }

        prevMouseX = mouseX;
        prevMouseY = mouseY;

		return true;
    }
};

int main()
{
    Main main;

    if(main.Construct(448, 448, 2, 2))
    {
        main.Start();
    }

    return 0;
}
