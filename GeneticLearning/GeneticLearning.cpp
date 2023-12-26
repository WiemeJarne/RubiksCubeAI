// GeneticLearning.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <windows.h>
#include "RubiksCube.hpp"
#include "GeneticAlgorithm.hpp"

void MoveCursor(int x, int y)
{
    static HANDLE h = NULL;
    if (!h)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(h, c);
}

bool SolveCube(bool printScrambe)
{
    CubeState target{};
    int populationMaxSize{ 1000 };
    float mutationRate{ 0.2f };
    int turns{ 50 };
    int restictedTurns{ 40 };
    int totalGenerations{};

    std::mt19937 generator
    (
        static_cast<unsigned int>
        (
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
            )
    );

    GeneticAlgorithm algorithm{ turns, target, mutationRate, populationMaxSize, generator };
    algorithm.AdjustRestictedTurns(restictedTurns);

    bool stop{};

    CubeState currBestCube{}, prevBestCube{};

    int highestFitness{};

    int generationsOfStagnation{};

    if (printScrambe)
    {
        std::cout << "To solve scramble: " << algorithm.m_OgScramble << '\n';
        std::cout << "Fitness goal: " << algorithm.m_PerfectScore << '\n';
    }

    while (!stop)
    {
        DNA best{ algorithm.GetBest() };
        prevBestCube = currBestCube;
        currBestCube = best.m_Cube;

        if (totalGenerations > 2)
        {
            if (highestFitness < best.m_Fitness)
                highestFitness = best.m_Fitness;

            MoveCursor(0, 3);

            std::cout << "Best current fitness: " << best.m_Fitness << '\n';
            std::cout << "Amount of turns: " << turns - restictedTurns - std::count(best.m_Genes.begin(), best.m_Genes.begin() + turns - restictedTurns, '\'') << '\n';
            std::cout << "Generation: " << totalGenerations << '\n';
            std::cout << "Best solve: " << best.m_Genes.substr(0, turns - restictedTurns) << '\n';
        }

        if (!algorithm.m_Finnished)
        {
            algorithm.NaturalSelection();
            algorithm.Generate();
            algorithm.CalculateFitness();

            ++totalGenerations;

            if (totalGenerations >= 3000)
                return false;

            if (currBestCube == prevBestCube)
            {
                ++generationsOfStagnation;

                if (generationsOfStagnation >= 500)
                {
                    if (restictedTurns == 0)
                    {
                        turns = 25;
                        restictedTurns = 10;
                        algorithm = GeneticAlgorithm(turns, target, mutationRate, populationMaxSize, generator);
                        algorithm.AdjustRestictedTurns(restictedTurns);
                    }

                    generationsOfStagnation = 0;
                    algorithm.AdjustRestictedTurns(--restictedTurns);
                }
            }
            else
            {
                generationsOfStagnation = 0;
            }
        }
        else
        {
            std::cout << "Finnished at generation: " << totalGenerations << '\n';

            //try to solve the cube with the found solution
            CubeState toSolveCube{};

            toSolveCube.Scramble(algorithm.m_OgScramble);

            toSolveCube.Scramble(std::string(best.m_Genes.begin(), best.m_Genes.begin() + turns - restictedTurns));

            std::cout << "Genes of best cube: " << best.m_Genes << '\n';

            std::cout << "Solution was: " << static_cast<char>(toSolveCube == target);

            return true;
        }
    }
}

int main()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int amountOfTries{};

    bool stop{};

    CubeState cube{};

    while (!stop)
    {
        if (amountOfTries == 0)
            stop = SolveCube(true);
        else stop = SolveCube(false);

        ++amountOfTries;

        MoveCursor(0, 2);

        std::cout << "Busy with try " << amountOfTries << '\n';
    }

    std::string input{};
    std::cout << "Yeeeeey solved cube after " << amountOfTries << " getting to 5000 generations after each try.\n";
    std::cin >> input;
}