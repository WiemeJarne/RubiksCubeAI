#include "RubiksCube.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <deque>
#include <fstream>
#include <boost/archive/text_oarchive.hpp> // For saving
#include <boost/archive/text_iarchive.hpp> // For loading
#include <conio.h> // For _getch() function
#include <chrono>

//statics
std::unique_ptr<CubeState> CubeState::solvedState;
bool CubeState::isCreatingSolvedState;

// create aliases for state and action:
using State = relearn::state<CubeState>;
using Action = relearn::action<CubeAction>;

void SaveAgent(const relearn::policy<State, Action>& policies)
{
    std::cout << "Saving agent to agent.policy\n";

    std::ofstream ofs("agent.policy");
    boost::archive::text_oarchive oa(ofs);
    oa << policies;

    std::cout << "Agent saved to agent.policy\n";
}

relearn::policy<State, Action>& LoadAgent()
{
    relearn::policy<State, Action> policies{};

    std::ifstream ifs("agent.policy");
    boost::archive::text_iarchive ia(ifs);
    ia >> policies;

    return policies;
}

void TrainAgent()
{
    //store policies and episodes
    relearn::policy<State, Action> policies;

    std::vector<std::deque<relearn::link<State, Action>>> episodes{};

    // Number of episodes to generate
    int amountOfEpisodes{ 500'000 };

    int amountOfMovesForCurrentEpisode{};

    constexpr int maxMovesPerEpisode{ 100 };

    std::cout << "Starting exploration\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    //Start the exploration phase
    for (int episodeNr = 0; episodeNr < amountOfEpisodes; ++episodeNr)
    {
        std::cout << "episode: " << episodeNr << '\n';

        amountOfMovesForCurrentEpisode = 0;

        // Create a starting state for the episode with scrambling
        CubeState current{ true };
        State stateNow{ State(current) };

        // Create a new episode (relearn::markov_chain)
        std::deque<relearn::link<State, Action>> episode;

        // Perform offline exploration until a solved state is found
        while (!current.IsSolved() && amountOfMovesForCurrentEpisode < maxMovesPerEpisode)
        {
            ++amountOfMovesForCurrentEpisode;

            // Randomly pick an action
            bool direction{};
            CubeAction action = CubeAction();

            current.DoAction(action);

            // Create the action using CubeAction as trait
            Action actionNow = Action(action);

            // Add the state to the episode
            episode.emplace_back(relearn::link<State, Action>{stateNow, actionNow});

            // update current state to next state
            stateNow = State(current.CalculateReward(), current);
        }

        // Store the episode in the list of experienced episodes
        episodes.emplace_back(episode);
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    auto explorationTime{ std::chrono::duration_cast<std::chrono::duration<double>>((endTime - startTime)).count() };

    std::cout << "Time taken for exploration: " << explorationTime << '\n';

    // Q-learning parameters
    double learningRate = 0.9; // Learning rate (alpha)
    double discountRate = 0.9; // Discount rate (gamma)

    // Create a Q-learning agent
    relearn::q_learning<State, Action> learner{ learningRate, discountRate };

    std::cout << "Start training the agent with the exploration data\n";

    startTime = std::chrono::high_resolution_clock::now();

    // Train the agent on the collected episodes
    for (int index = 0; index < 10; ++index)
    {
        std::cout << "Train loop " << index << " started\n";
        for (auto& episode : episodes)
            learner(episode, policies);
    }

    endTime = std::chrono::high_resolution_clock::now();

    std::cout << "Time taken for training: " << explorationTime + std::chrono::duration_cast<std::chrono::duration<double>>((endTime - startTime)).count() << '\n';

    SaveAgent(policies);
}

void UseAgent()
{
    relearn::policy<State, Action> policies = LoadAgent();

    // Create a CubeState to represent the starting state
    CubeState cubeToSolve(true);

    // Define the maximum number of steps to solve the cube (adjust as needed)
    int max_steps = 100;

    // Follow the best policy to solve the cube
    for (int step = 0; step < max_steps; ++step)
    {
        // Check if the cube is already solved
        if (cubeToSolve.IsSolved()) 
        {
            std::cout << "Cube is solved!\n";
            break;
        }

        // Get the best action based on the current state from policies
        auto bestAction = policies.best_action(State(cubeToSolve));
        
        // Apply the best action to the state
        if (bestAction)
        {
            cubeToSolve.DoAction(bestAction.get()->trait().action);
            std::cout << "Best action found\n";
        }
    }

    // Check if the cube is solved after the maximum number of steps
    if (!cubeToSolve.IsSolved())
        std::cout << "Cube could not be solved within the maximum number of steps.\n";
}

int main()
{
    //TrainAgent();

    UseAgent();

    return 0;
}