#include <iostream>
#include <vector>

int main()
{
	std::vector<int> intVector;
	intVector.push_back(1);
	
	// We get the pointer to the first element from our vector.
	int* pointerToInt = &intVector[0];
	std::cout << "The value of our int is: " << *pointerToInt << std::endl;
	
	// Add two more elements to trigger vector resize. During
	// resize the internal array is deleted causing our pointer
	// to point to an invalid location.
	intVector.push_back(2);
	intVector.push_back(3);
	std::cout << "The value of our int is: " << *pointerToInt << std::endl;

	return 0;
}