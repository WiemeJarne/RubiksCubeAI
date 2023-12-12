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

// policy memory
relearn::policy<State, Action> policies;

void SaveAgent()
{
    std::cout << "Saving agent to agent.policy\n";

    std::ofstream ofs("agent.policy");
    boost::archive::text_oarchive oa(ofs);
    oa << policies;

    std::cout << "Agent saved to agent.policy\n";
}

void LoadAgent()
{
    std::ifstream ifs("agent.policy");
    boost::archive::text_iarchive ia(ifs);
    ia >> policies;
}

CubeActionPosibilities getRandomAction(std::mt19937& gen, bool& outDirection) {
    // Generate a random number in the range [0, 5] to represent each action
    int randomNr = std::uniform_int_distribution<int>(0, 5)(gen);

    outDirection = rand() % 2;

    return static_cast<CubeActionPosibilities>(randomNr);
}

double CalculateReward(const CubeState& currentState, CubeActionPosibilities action, const CubeState& nextState)
{
    //if a piece is on the correct place in the nextState reward = 2
    //if a piece is not in the correct place anymore in the nextState reward = -1
    double rewardForCorrectPlace = 1;
    double rewardForNotCorrectPlaceAnyMore = -1;

    double reward{};

    for (int index{}; index < currentState.pieces.size(); ++index)
    {
        if (nextState.pieces[index] == nextState.solvedState->pieces[index])
        {
            reward += rewardForCorrectPlace;
        }

        if (currentState.pieces[index] == nextState.solvedState->pieces[index]
            && nextState.pieces[index] != nextState.solvedState->pieces[index])
        {
            reward += rewardForNotCorrectPlaceAnyMore;
        }
    }

    return reward;
}

int main()
{
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a list to store episodes
    std::vector<std::deque<relearn::link<State, Action>>> episodes;

    // Number of episodes to generate
    int num_episodes = 1000;

    int amountOfMovesForCurrentEpisode{};

    constexpr int maxMovesPerEpisode{ 100 };

    std::cout << "Starting exploration\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    //Start the exploration phase
    for (int episode_num = 0; episode_num < num_episodes; ++episode_num) 
    {
        amountOfMovesForCurrentEpisode = 0;

        // Create a starting state for the episode with scrambling
        CubeState currentState{ true };

        // Create a new episode (relearn::markov_chain)
        std::deque<relearn::link<State, Action>> episode;

        // Perform offline exploration until a solved state is found
        while (!currentState.IsSolved() && amountOfMovesForCurrentEpisode < maxMovesPerEpisode)
        {
            // Randomly pick an action
            bool direction{};
            CubeActionPosibilities action = getRandomAction(gen, direction);

            ++amountOfMovesForCurrentEpisode;

            // Apply the action to the state
            CubeState nextState{ currentState };
            nextState.DoAction(action, direction);
            
            double reward{ CalculateReward(currentState, action, nextState) };

            // Create a link for the episode
            State state{ reward, currentState };
            relearn::link<State, Action> link{ state, CubeAction{ action } };

            // Add the link to the episode
            episode.push_back(link);
        }

        // Store the episode in the list of experienced episodes
        episodes.push_back(episode);
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    auto explorationTime{ std::chrono::duration_cast<std::chrono::duration<double>>((endTime - startTime)).count() };

    std::cout << "Time taken for exploration: " << explorationTime << '\n';

    // Q-learning parameters
    double learningRate = 0.1; // Learning rate (alpha)
    double discountRate = 0.9; // Discount rate (gamma)

    // Create a Q-learning agent
    relearn::q_learning<State, Action> learner{ learningRate, discountRate };

    std::cout << "Start training the agent with the exploration data\n";

    startTime = std::chrono::high_resolution_clock::now();

    // Train the agent on the collected episodes
    for (int index = 0; index < 10; ++index)
    {
        for (auto episode : episodes)
        {
            learner(episode, policies);
        }
    }

    endTime = std::chrono::high_resolution_clock::now();

    std::cout << "Time taken for training: " << explorationTime + std::chrono::duration_cast<std::chrono::duration<double>>((endTime - startTime)).count() << '\n';

    SaveAgent();

    // Once the policies are updated, you can follow the best policy to solve the cube
    // Call the on_policy function here

    return 0;
}