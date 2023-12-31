// GeneticLearning.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <windows.h>
#include <fstream>
#include <string>
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

int SolveCube(bool printScrambe, std::mt19937& generator, const std::string& scramble, int turns, std::chrono::steady_clock::time_point& timePointHighestFitness)
{
    CubeState target{};
    int populationMaxSize{ 1000 };
    float mutationRate{ 0.2f };
    int totalGenerations{};
    int maxGenerationNr{ 1000 };

    GeneticAlgorithm algorithm{ turns, target, mutationRate, populationMaxSize, generator, scramble };

    CubeState currBestCube{}, prevBestCube{};

    int highestFitness{};

    int generationsOfStagnation{};

    /*if (printScrambe)
    {
        std::cout << "To solve scramble: " << algorithm.GetScramble() << '\n';
        std::cout << "Fitness goal: " << algorithm.GetPerfectScore() << '\n';
    }*/

    while (true)
    {
        DNA best{ algorithm.GetBest() };
        prevBestCube = currBestCube;
        currBestCube = best.GetCubeState();

        //MoveCursor(0, 3);

        if (totalGenerations > 2)
        {
            if (highestFitness < best.GetFitness())
            {
                highestFitness = best.GetFitness();
                timePointHighestFitness = std::chrono::high_resolution_clock::now();
            }

            //std::cout << "Best current fitness: " << best.GetFitness() << '\n';
            //std::cout << "Amount of turns: " << best.GetTurns() - std::count(best.GetGenes().begin(), best.GetGenes().begin() + best.GetTurns(), '\'') << '\n';
            //std::cout << "Generation: " << totalGenerations << '\n';
            //std::cout << "Best solve: " << best.GetGenes().substr(0, best.GetTurns()) << '\n';
        }
        /*else
        {
            std::cout << "                                                         \n";
            std::cout << "                                                         \n";
            std::cout << "                                                         \n";
            std::cout << "                                                         \n";
        }*/

        if (!algorithm.GetIsFinnished())
        {
            algorithm.NaturalSelection();
            algorithm.Generate(turns);
            algorithm.CalculateFitness();

            ++totalGenerations;

            if (totalGenerations == maxGenerationNr)
                return highestFitness;
        }
        else
        {
            //std::cout << "Finnished at generation: " << totalGenerations << '\n';

            //try to solve the cube with the found solution
            CubeState toSolveCube{};

            toSolveCube.Scramble(algorithm.GetScramble());

            toSolveCube.Scramble(std::string(best.GetGenes().begin(), best.GetGenes().begin() + turns));

            //std::cout << "Genes of best cube: " << best.GetGenes() << '\n';

            //std::cout << "Solution was: " << static_cast<char>(toSolveCube == target);

            return highestFitness;
        }
    }

    return false;
}

void WriteResultsToFile(std::ofstream& file, int highestFitness, double duration, int attemptNr, const std::string& scramble)
{
    if (!file.is_open())
        return;

    //if (attemptNr == 0)
        //     file << "\nScramble: " << scramble << '\n';
    auto durationStr{ std::to_string(duration) };
    auto dotIterator{ std::find(durationStr.begin(), durationStr.end(), '.') };
    durationStr.replace(dotIterator, dotIterator + 1, ",");
    file << '"' << attemptNr << '"' << ';' << '"' << highestFitness << '"' << ';' << '"' << durationStr << '"' << "\n";

    //file << "AttemptNr: " << attemptNr << '\n';
    //file << "Highest fitness: " << highestFitness << '\n';
    //file << "Duration: " << duration << '\n';
}

int main()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    CubeState cube{};

    std::mt19937 generator
    (
        static_cast<unsigned int>
        (
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );

    DNA::SetGenerator(generator);

    std::ofstream outputFile{ "GeneticLearningResults.csv" };

    int scrambleNr{};

    int turns{ 30 };

    int highestFitness{};

    std::chrono::steady_clock::time_point timePointHighestFitness{}; //this is the time point when the highest fitness was reached

    while (scrambleNr < 100)
    {
        int attemptNr{};

        std::string scramble{ CubeState().GenerateScramble(turns, generator) };

        while (attemptNr < 10)
        {
            std::cout << "Bussy with scramble " << scrambleNr << " attempt " << attemptNr << '\n';

            auto startTime = std::chrono::high_resolution_clock::now();

            highestFitness = SolveCube(false, generator, scramble, turns, timePointHighestFitness);

            auto endTime = std::chrono::high_resolution_clock::now();

            auto duration{ std::chrono::duration_cast<std::chrono::duration<double>>((timePointHighestFitness - startTime)).count() };

            WriteResultsToFile(outputFile, highestFitness, duration, attemptNr, scramble);

            ++attemptNr;
        }

        ++scrambleNr;
    }

    outputFile.close();

    std::string input{};

    std::cout << "done\n";
    std::cin >> input;
}