#ifndef RUBIKSCUBE_HPP
#define RUBIKSCUBE_HPP
//#define USING_BOOST_SERIALIZATION
#include <iostream>
#include <vector>
#include <random>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

//this is for a 2x2x2 cube

enum class Color
{
	yellow = 0,
	white = 2,
	green = 4,
	blue = 8,
	orange = 16,
	red = 32,
	none = 64
};

struct Piece
{
	Piece(const std::vector<Color>& _colors)
	{
		colors = _colors;
	}

	std::vector<Color> colors{}; //left(0), right(1), front(2), back(3), top(4), bottom(5)

	void AddRotation(double radX, double radY, double radZ)
	{
		Color temp;

		if (radX == M_PI_2)
		{
			temp = colors[2];
			colors[2] = colors[4];
			colors[4] = colors[3];
			colors[3] = colors[5];
			colors[5] = temp;
		}
		else if (radX == -M_PI_2)
		{
			temp = colors[5];
			colors[5] = colors[3];
			colors[3] = colors[4];
			colors[4] = colors[2];
			colors[2] = temp;
		}
		else if (radY == M_PI_2)
		{
			temp = colors[0];
			colors[0] = colors[3];
			colors[3] = colors[1];
			colors[1] = colors[2];
			colors[2] = temp;
		}
		else if (radY == -M_PI_2)
		{
			temp = colors[2];
			colors[2] = colors[1];
			colors[1] = colors[3];
			colors[3] = colors[0];
			colors[0] = temp;
		}
		else if (radZ == M_PI_2)
		{
			temp = colors[0];
			colors[0] = colors[4];
			colors[4] = colors[1];
			colors[1] = colors[5];
			colors[5] = temp;
		}
		else if (radZ == -M_PI_2)
		{
			temp = colors[5];
			colors[5] = colors[1];
			colors[1] = colors[4];
			colors[4] = colors[0];
			colors[0] = temp;
		}
	}

	void Print()
	{
		if(ColorToString(colors[0]) != "none")
			std::cout << "left: " << ColorToString(colors[0]);

		if (ColorToString(colors[1]) != "none")
			std::cout << " right: " << ColorToString(colors[1]);

		if (ColorToString(colors[2]) != "none")
			std::cout << " front: " << ColorToString(colors[2]);

		if (ColorToString(colors[3]) != "none")
			std::cout << " back: " << ColorToString(colors[3]);
		
		if (ColorToString(colors[4]) != "none")
			std::cout << " top: " << ColorToString(colors[4]);
		
		if (ColorToString(colors[5]) != "none")
			std::cout << " bottom: " << ColorToString(colors[5]);
	}

	bool operator==(const Piece rhs) const
	{
		return colors == rhs.colors;
	}

	bool operator!=(const Piece rhs) const
	{
		return colors != rhs.colors;
	}

	std::string ColorToString(Color color)
	{
		switch (color)
		{
		case Color::yellow:
			return "yellow";
		case Color::white:
			return "white";
		case Color::green:
			return "green";
		case Color::blue:
			return "blue";
		case Color::orange:
			return "orange";
		case Color::red:
			return "red";
		case Color::none:
			return "none";
		}

		return "error";
	}
};

//the pieces are oredered like this:
//m_Pieces[0] = row 0, col 0, depth 0 = left top front
//m_Pieces[1] = row 0, col 0, depth 1 = left top back
//m_Pieces[2] = row 0, col 1, depth 0 = left bottom front
//m_Pieces[3] = row 0, col 1, depth 1 = left bottom back
//m_Pieces[4] = row 1, col 0, depth 0 = right top front
//m_Pieces[5] = row 1, col 0, depth 1 = right top back
//m_Pieces[6] = row 1, col 1, depth 0 = right bottom front
//m_Pieces[7] = row 1, col 1, depth 1 = right bottom back
//row = right or left
//col = top or bottom
//depth = front or back
//KEEP IN MIND -> GREEN = FRONT AND YELLOW = TOP IN A SOLVED CUBE

struct CubeState
{
	std::vector<std::shared_ptr<Piece>> pieces{};

	CubeState()
	{
		for (int x{}; x < 2; ++x)
		{
			for (int y{}; y < 2; ++y)
			{
				for(int z{}; z < 2; ++z)
				{
					const std::vector<Color> colors
					{
						(x == 0) ? Color::orange : Color::none,
						(x == 1) ? Color::red : Color::none,
						(z == 0) ? Color::green : Color::none,
						(z == 1) ? Color::blue : Color::none,
						(y == 0) ? Color::white : Color::none,
						(y == 1) ? Color::yellow : Color::none
					};
					
					pieces.push_back(std::make_shared<Piece>(colors));
				}
			}
		}
	}

	void Print()
	{
		std::cout << "left top front: ";
		std::cout << '\n';
		pieces[0]->Print();
		std::cout << "\n\n";

		std::cout << "left top back: ";
		std::cout << '\n';
		pieces[1]->Print();
		std::cout << "\n\n";

		std::cout << "left bottom front: ";
		std::cout << '\n';
		pieces[2]->Print();
		std::cout << "\n\n";

		std::cout << "left bottom back: ";
		std::cout << '\n';
		pieces[3]->Print();
		std::cout << "\n\n";

		std::cout << "right top front: ";
		std::cout << '\n';
		pieces[4]->Print();
		std::cout << "\n\n";

		std::cout << "right top back: ";
		std::cout << '\n';
		pieces[5]->Print();
		std::cout << "\n\n";

		std::cout << "right bottom front: ";
		std::cout << '\n';
		pieces[6]->Print();
		std::cout << "\n\n";

		std::cout << "right bottom back: ";
		std::cout << '\n';
		pieces[7]->Print();
		std::cout << "\n\n";
	}

	void Scramble(const std::string& _scramble)
	{
		int currentIndex{};

		while (currentIndex < _scramble.size())
		{
			if (currentIndex < _scramble.size() - 1 && _scramble[currentIndex + 1] == '\'')
			{
				DoAction(_scramble.substr(currentIndex, 2));
				currentIndex += 2;
			}
			else
			{
				DoAction(_scramble.substr(currentIndex, 1));
				++currentIndex;
			}
		}
	}

	const std::string GenerateScramble(int amountOfTurns, std::mt19937& generator)
	{
		std::string scrambleString{};

		std::string secondToLastAction{};
		std::string lastAction{};
		std::string action{};

		for (int index{}; index < amountOfTurns; ++index)
		{
			action = GetRandomAction(generator);

			while (index == 0 && action.find('\'') != std::string::npos)
				action = GetRandomAction(generator);

			if (index < 2)
			{
				//make sure the 2 first actions are not the same action but in the opposite direction
				if (lastAction.size() != 0)
				{
					while (action[0] == lastAction[0] && action.find('\'') != std::string::npos)
						action = GetRandomAction(generator);
				}

				scrambleString += action;

				secondToLastAction = lastAction;
				lastAction = action;
				continue;
			}

			//make sure there are no 3 of the same actions after each other and make sure there are no ' actions next to each other
			//and make sure the same action but in the opposite direction are after each other
			while ((action == secondToLastAction && (action == lastAction || lastAction.find('\'') != std::string::npos)) || (action.find('\'') != std::string::npos && lastAction.find('\'') != std::string::npos))
				action = GetRandomAction(generator);

			scrambleString += action;

			secondToLastAction = lastAction;
			lastAction = action;
		}

		return scrambleString;
	}

	std::string GetRandomAction(std::mt19937& generator)
	{
		std::string rotations[]{ "F", "F'", "B", "B'", "U", "U'", "D", "D'", "L", "L'", "R", "R'" };

		std::uniform_int_distribution<unsigned int> dist(0, 11);

		return rotations[dist(generator)];
	}

	void DoAction(const std::string& action)
	{
		bool isCCW{ true };

		if (action.size() == 2)
			isCCW = false;

		if (action[0] == 'R')
			RotateRight(isCCW);
		else if (action[0] == 'L')
			RotateLeft(isCCW);
		else if (action[0] == 'U')
			RotateTop(isCCW);
		else if (action[0] == 'D')
			RotateBottom(isCCW);
		else if (action[0] == 'F')
			RotateFront(isCCW);
		else if (action[0] == 'B')
			RotateBack(isCCW);
	}

	void RotateRight(bool clockWise)
	{
		auto tempPiece = pieces[4];

		if (clockWise)
		{
			pieces[4] = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = tempPiece;

			pieces[4]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[6]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[7]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[5]->AddRotation(-M_PI_2, 0.f, 0.f);
		}
		else
		{
			tempPiece = pieces[5];
			pieces[5] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = pieces[4];
			pieces[4] = tempPiece;

			pieces[5]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[7]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[6]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[4]->AddRotation(M_PI_2, 0.f, 0.f);
		}
	}

	void RotateLeft(bool clockWise)
	{
		auto tempPiece = pieces[1];

		if (clockWise)
		{
			pieces[1] = pieces[3];
			pieces[3] = pieces[2];
			pieces[2] = pieces[0];
			pieces[0] = tempPiece;

			pieces[1]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[3]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[2]->AddRotation(M_PI_2, 0.f, 0.f);
			pieces[0]->AddRotation(M_PI_2, 0.f, 0.f);
		}
		else
		{
			tempPiece = pieces[0];
			pieces[0] = pieces[2];
			pieces[2] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;

			pieces[0]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[2]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[3]->AddRotation(-M_PI_2, 0.f, 0.f);
			pieces[1]->AddRotation(-M_PI_2, 0.f, 0.f);
		}
	}

	void RotateFront(bool clockWise)
	{
		auto tempPiece = pieces[0];

		if (clockWise)
		{
			pieces[0] = pieces[2];
			pieces[2] = pieces[6];
			pieces[6] = pieces[4];
			pieces[4] = tempPiece;

			pieces[0]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[2]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[6]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[4]->AddRotation(0.f, 0.f, -M_PI_2);
		}
		else
		{
			tempPiece = pieces[4];
			pieces[4] = pieces[6];
			pieces[6] = pieces[2];
			pieces[2] = pieces[0];
			pieces[0] = tempPiece;

			pieces[0]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[2]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[6]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[4]->AddRotation(0.f, 0.f, M_PI_2);
		}
	}

	void RotateBack(bool clockWise)
	{
		auto tempPiece = pieces[5];

		if (clockWise)
		{
			pieces[5] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = pieces[1];
			pieces[1] = tempPiece;

			pieces[1]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[3]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[7]->AddRotation(0.f, 0.f, M_PI_2);
			pieces[5]->AddRotation(0.f, 0.f, M_PI_2);
		}
		else
		{
			tempPiece = pieces[1];
			pieces[1] = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[5];
			pieces[5] = tempPiece;

			pieces[1]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[3]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[7]->AddRotation(0.f, 0.f, -M_PI_2);
			pieces[5]->AddRotation(0.f, 0.f, -M_PI_2);
		}
	}

	void RotateTop(bool clockWise)
	{
		auto tempPiece = pieces[4];

		if (clockWise)
		{
			pieces[4] = pieces[5];
			pieces[5] = pieces[1];
			pieces[1] = pieces[0];
			pieces[0] = tempPiece;

			pieces[0]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[1]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[5]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[4]->AddRotation(0.f, -M_PI_2, 0.f);
		}
		else
		{
			tempPiece = pieces[0];
			pieces[0] = pieces[1];
			pieces[1] = pieces[5];
			pieces[5] = pieces[4];
			pieces[4] = tempPiece;

			pieces[0]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[1]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[5]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[4]->AddRotation(0.f, M_PI_2, 0.f);
		}
	}

	void RotateBottom(bool clockWise)
	{
		auto tempPiece = pieces[2];

		if (clockWise)
		{
			pieces[2] = pieces[3];
			pieces[3] = pieces[7];
			pieces[7] = pieces[6];
			pieces[6] = tempPiece;

			pieces[2]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[3]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[6]->AddRotation(0.f, M_PI_2, 0.f);
			pieces[7]->AddRotation(0.f, M_PI_2, 0.f);
		}
		else
		{
			tempPiece = pieces[6];
			pieces[6] = pieces[7];
			pieces[7] = pieces[3];
			pieces[3] = pieces[2];
			pieces[2] = tempPiece;			

			pieces[6]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[7]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[3]->AddRotation(0.f, -M_PI_2, 0.f);
			pieces[2]->AddRotation(0.f, -M_PI_2, 0.f);
		}
	}

	bool operator==(const CubeState rhs) const
	{
		const int amountOfPieces{ static_cast<int>(pieces.size()) };
		for (int index{}; index < amountOfPieces; ++index)
		{
			if (pieces[index] != rhs.pieces[index])
				return false;
		}

		return true;
	}
};
#endif // RUBIKSCUBE_HPP