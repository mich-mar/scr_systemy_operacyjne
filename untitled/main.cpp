#include <iostream>
#include <vector>

std::vector<int> findAllDeviders(int num)
{
	std::vector<int> deviders;
	for (int i=0; i <= num/2; i++ ){
		if(num%i == 0)
		{
			deviders.push_back(i);
		}
	}

	return deviders;
}

bool isEqualToSum(int num)
{
	std::vector<int> deviders = findAllDeviders(num);
	int sum = 0;

	for (int i=0; i < deviders.size(); i++ ){
		sum += deviders.at(i);
	}

	return (sum == num);
}

int main() {
	std::string input;
	std::cin >> input;    //Reading input from STDIN

	int N = stoi(input);

	std::string answer;

	if(isEqualToSum(N))
		answer = "YES";
	else
		answer = "NO";

	std::cout << answer;

	return 0;
}