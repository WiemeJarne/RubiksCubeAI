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

relearn::policy<State, Action> LoadAgent()
{
    std::cout << "Loading agent from agent.policy\n";

    relearn::policy<State, Action> policies{};

    std::ifstream ifs("agent.policy");
    boost::archive::text_iarchive ia(ifs);
    ia >> policies;

    std::cout << "Successfully loaded agent from agent.policy\n";

    return policies;
}

void TrainAgent(bool trainNewAgent) //when true then the previous trained agent will be overridden
{
    //store policies and episodes
    relearn::policy<State, Action> policies;

    if (!trainNewAgent)
        policies = LoadAgent();

    std::mt19937 generator
    (
        static_cast<unsigned int>
        (
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );

    std::vector<std::deque<relearn::link<State, Action>>> episodes{};

    // Number of episodes to generate
    int amountOfEpisodes{ 500'000 };

    int maxAmountOfMovesPerEpisode{ 100 };
    
    int amountOfMovesInCurrentEpisode{};

    std::cout << "Starting exploration\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    //Start the exploration phase
    for (int episodeNr{ 0 }; episodeNr < amountOfEpisodes; ++episodeNr)
    {
        std::cout << "episode: " << episodeNr << '\n';

        amountOfMovesInCurrentEpisode = 0;

        // Create a starting state for the episode with scrambling
        CubeState current{ true, generator };
        State stateNow{ State(current) };
        
        CubeState next{ current };

        // Create a new episode (relearn::markov_chain)
        std::deque<relearn::link<State, Action>> episode;

        bool stop = false;

        // Explore while Reward is zero
        // and keep populating the episode with states and actions
        while (!current.IsSolved() && !stop && amountOfMovesInCurrentEpisode < maxAmountOfMovesPerEpisode)
        {
            ++amountOfMovesInCurrentEpisode;

            // Randomly pick an action
            CubeAction action = CubeAction(generator);

            next.DoAction(action);

            // Create the action using CubeAction as trait
            Action actionNow = Action(action);

            // Add the state to the episode
            episode.emplace_back(relearn::link<State, Action>{stateNow, actionNow});

            // update current state to next state if the reward was not zero
            if (next.reward != 0)
            {
                stateNow = State(next.reward, current);
                stop = true;
            }
            // if the reward was zero set next back to current and try again until a non zero reward is found for current
            else next = current;
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

        //save the agent after each loop to be safe
        SaveAgent(policies);
    }

    endTime = std::chrono::high_resolution_clock::now();

    std::cout << "Time taken for training: " << explorationTime + std::chrono::duration_cast<std::chrono::duration<double>>((endTime - startTime)).count() << '\n';

    SaveAgent(policies);
}

void UseAgent()
{
    std::mt19937 generator
    (
        static_cast<unsigned int>
        (
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );

    relearn::policy<State, Action> policies = LoadAgent();

    // Create a CubeState to represent the starting state
    CubeState cubeToSolve{ true, generator };

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
        //otherwise do a random action
        else
        {
            cubeToSolve.DoAction(CubeAction(generator));
            std::cout << "No best action found, doing random action\n";
        }
    }

    // Check if the cube is solved after the maximum number of steps
    if (!cubeToSolve.IsSolved())
        std::cout << "Cube could not be solved within the maximum number of steps.\n";
}

int main()
{
    TrainAgent(false);

    //UseAgent();

    return 0;
}