#include <iostream>
#include <Windows.h>
#include <chrono>


int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 10.0f;
float fPlayerY = 2.0f;
float fPlayerA = 0.0f;

float nMapHeight = 20;
float nMapWidth = 20;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;
int main()
{

    // Create screen buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight]; // create screen of type wchar
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // handle (textmode buffer)
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0; // useless variable

    std::wstring map;

    map += L"####################";
    map += L"######........######";
    map += L"######........######";
    map += L"######........######";
    map += L"######........######";
    map += L"#########.##########";
    map += L"#########.##########";
    map += L"#########.##########";
    map += L"#...##.............#";
    map += L"#...##.............#";
    map += L"#..........#......##";
    map += L"#................###";
    map += L"#.......###.....####";
    map += L"#.......#........###";
    map += L"#.......#.........##";
    map += L"#..................#";
    map += L"#..................#";
    map += L"#....#.............#";
    map += L"#....#.............#";
    map += L"####################";
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    while (1)
    {
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();
      
        for (int x = 0; x < nScreenWidth; x++)
        {
            // reset
            if (GetAsyncKeyState((unsigned short)'R') & 0x8000)
            {
                fPlayerX = 8.0;
                fPlayerY = 8.0;
                fPlayerA = 0;
            }
            //Controls
            if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            {
                fPlayerA -= 0.015 * fElapsedTime;
            }
            if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            {
                fPlayerA += 0.015 * fElapsedTime;
            }

            if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
            {
                fPlayerX += sinf(fPlayerA) * 0.1f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 0.1f * fElapsedTime;
                if(map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                {
                    fPlayerX -= sinf(fPlayerA) * 0.1f * fElapsedTime;
                    fPlayerY -= cosf(fPlayerA) * 0.1f * fElapsedTime;
                }
            }
            if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
            {
                fPlayerX -= sinf(fPlayerA) * 0.1f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 0.1f * fElapsedTime;
                if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
                {
                    fPlayerX += sinf(fPlayerA) * 0.1f * fElapsedTime;
                    fPlayerY += cosf(fPlayerA) * 0.1f * fElapsedTime;
                }
            }

            //coords are out of bounds

       

            //Calculate the projected ray angle into space
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;
            float fDistance = 0;
            bool bHitWall = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistance < fDepth)
            {
                fDistance += 0.1f;
                int nTestX = (int)(fPlayerX + fEyeX * fDistance);
                int nTestY = (int)(fPlayerY + fEyeY * fDistance);

                //Test if ray is out of bounds

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistance = fDepth;
                }
                else
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;
                    }
                }
            }

                //Calculate player distance from ceiling and floor
                int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistance);
                int nFloor = nScreenHeight - nCeiling;

                short nShade = ' ';
 if (fDistance <= fDepth / 4.0f)     nShade = 0x2588; // Very close
                else if (fDistance < fDepth / 3.0f) nShade = 0x2593;
                else if (fDistance < fDepth / 2.0)  nShade = 0x2592;
                else if (fDistance < fDepth)        nShade = 0x2591;
                else  nShade = ' '; // Out of view
 short fnShade;

               
                for (int y = 0; y < nScreenHeight; y++)
                {
                    if (y < nCeiling)
                        screen[y * nScreenWidth + x] = ' ';
                    else if (y > nCeiling && y <= nFloor)
                    {
                        screen[y * nScreenWidth + x] = nShade;
                    }
                    else
                    {
                        float b = 1.0 - (((float)y - nScreenHeight / 2.0) / ((float)nScreenHeight / 2.0f));
                        if (b < 0.25)       fnShade = '#';//close
                        else if (b < 0.5)   fnShade = 'x';
                        else if (b < 0.75)  fnShade = '.';
                        else if (b < 0.9)   fnShade = '-';
                        else                fnShade = ' '; // too far
                        screen[y * nScreenWidth + x] = fnShade;
                    }
                }

            }

        screen[nScreenWidth * nScreenHeight - 1] = '\0'; // Sets the limit
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
        }

    


}
