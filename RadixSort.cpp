#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

class listNode{
	friend class linkedListStack;
	friend class linkedListQueue;
	friend class RadixSort;	
	private:
		int data;
		listNode* next;
	public:
		listNode(int d){
			data = d;
			next = NULL;
		}
};

class linkedListStack{
	friend class RadixSort;
	private:
		listNode* Top;
	public:
		linkedListStack(){
			Top = NULL;
		}
		
		void Push(listNode* newNode){
			newNode->next = Top;
			Top = newNode;
		}
		
		listNode* Pop(){
			listNode* temp = Top;
			Top = Top->next;
			temp->next = NULL;
			return temp;
		}
		
		bool isEmpty(){
			if(Top != NULL)
				return 0;
			else return 1;
		}
		
		void printStack(ofstream &outFile){
			outFile << "***Below is the output of the stack***" << endl;
			outFile << "Top -> ";
			listNode* currentNode = Top;
			while (currentNode != NULL){
				outFile << "(" << currentNode->data << ", ";
				if(currentNode->next != NULL)
					outFile << currentNode->next->data << ") -> "; 
				else outFile << "NULL) ";
				currentNode = currentNode->next;
			}
			outFile << "-> NULL" << endl;
		}
};

class linkedListQueue{
	friend class RadixSort;	
	private:
		listNode* head;
		listNode* tail;
		listNode* dummy;
	
	public:
		linkedListQueue(){
			dummy = new listNode(0);
			head = dummy;
			tail = dummy;
		}
		
		void addTail(listNode* newNode){
			if(isEmpty())
				dummy->next = newNode;
			else tail->next = newNode;
			tail = newNode;
		}
		
		listNode* deleteFront(){
			listNode* temp = head->next;
			if(temp == tail)
				tail = head;
			head->next = temp->next;
			temp->next = NULL;
			return temp;
		}
		
		bool isEmpty(){
			if(tail == head)
				return 1;
			else return 0;
		}
		
		void printQueue(int index, ofstream& outFile){
			outFile << "*** Below is the output of the queue at index " << index << " .***" << endl;
			outFile << "Front (" << index << ") -> ";
			listNode* currentNode = head;	
			
			while(currentNode->next != NULL){
				currentNode = currentNode->next;
				outFile << "(" << currentNode->data << ", " ;
				if(currentNode->next != NULL)
					outFile << currentNode->next->data << ") -> ";
				else outFile << "NULL) ->";
			}
			outFile << "NULL" << endl;
			
			outFile << "Tail (" << index << ") -> ";
			if(!isEmpty())
				outFile << "(" << tail->data << ", " << "NULL) -> ";
			outFile << "NULL" << endl;
		}
};

class RadixSort{
	private:
		int tableSize;
		linkedListQueue** hashTable;
		int data;
		int currentTable;
		int previousTable;
		int maxDigits;
		int offSet;
		int currentDigit;
		linkedListStack Top;
	public:
		RadixSort(){
			tableSize = 10;
			hashTable = new linkedListQueue*[2];
			for(int i = 0; i < 2; i++){
					hashTable[i] = new linkedListQueue[tableSize];
			}
		}
	
		void firstReading(ifstream& inFile){
			int negativeNum = 0;
			int positiveNum = 0;
			while (inFile >> data){
				if(data < negativeNum)
					negativeNum = data;
				if(data > positiveNum)
					positiveNum = data;
			}
			offSet = abs(negativeNum);
			positiveNum += offSet;
			maxDigits = getMaxDigits(positiveNum);
		}
		
		void loadStack(ifstream& inFile, ofstream& outFile){
			while(inFile >> data){
				data += offSet;
				listNode* newNode = new listNode(data);
				Top.Push(newNode);
			}
			Top.printStack(outFile);
		}
		
		void dumpStack(ofstream& outFile){
			listNode* node;
			int digit;
			int hashIndex;

			while(!Top.isEmpty()){
				node = Top.Pop();
				digit = getOneDigit(node);
				hashIndex = digit;
				hashTable[currentTable][hashIndex].addTail(node);	
			}
			printTable(outFile);
		}
		
		void Sort(ofstream& outFile1, ofstream& outFile2){
			listNode* node;
			int digit;
			int hashIndex;
			int currentQueue;
			currentDigit = 0;
			currentTable = 0;
			previousTable = 1;
			
			dumpStack(outFile2);
	
			while(currentDigit < maxDigits){
				int temp = currentTable; 
				currentTable = previousTable; 
				previousTable = temp;
				currentQueue = 0;
				currentDigit++;
				while(currentQueue < tableSize){
					while(!hashTable[previousTable][currentQueue].isEmpty()){
						node = hashTable[previousTable][currentQueue].deleteFront();
						digit = getOneDigit(node);
						hashIndex = digit;
						hashTable[currentTable][hashIndex].addTail(node);
					}
					currentQueue++;			
				}			
			}
			addOffset();
			printTable(outFile2);
			printResult(outFile1);
		}
		
		int tableIndex(){
			return currentTable;
		}
		
		int getMaxDigits(int digit){
			int length = 0;
			while(digit!=0){
				length++;
				digit /= 10;
			}
			return length;
		}
		
		int getOneDigit(listNode* node){
			int nodeData = node->data;
			int nodeDigit = getHashIndex(nodeData);
			return nodeDigit;
		}
		
		int getHashIndex(int digit){
			int index = 0;
			for(int i = 0; i <= currentDigit; i++){
				index = digit%10;
				digit/=10;
			}
			return index;
		}
		
		void addOffset(){
			linkedListQueue finalQueue = hashTable[currentTable][0];
			listNode* currentNode = finalQueue.head;
			while(currentNode->next != NULL){
				currentNode = currentNode->next;
				currentNode->data -= offSet;	
			}
		}
		
		void printTable(ofstream& outFile){
			linkedListQueue currentQueue;
			listNode* currentNode;
			outFile << "***Below is the output of the table***" << endl;
			int j = 0;
			while(j < tableSize){
				currentQueue = hashTable[currentTable][j];
				currentNode = currentQueue.head;
				if(!currentQueue.isEmpty()){
					outFile << "Table [" << currentTable << "][" << j << "]: "; 
					while(currentNode->next != NULL){
						currentNode = currentNode->next;
						if(currentNode == currentQueue.tail)
							outFile << currentNode->data << endl;
						else outFile << currentNode->data << ", ";
					}
				}
				j++;
			}
		}
		
		void printResult(ofstream& outFile){
			linkedListQueue currentQueue = hashTable[currentTable][0];
			listNode* currentNode = currentQueue.head;
			while(currentNode->next != NULL){
				currentNode = currentNode->next;
				outFile << currentNode->data <<" ";
			}
			outFile << endl;
		}
};

int main(int argc, char** argv){
	ifstream inFile;
	ofstream outFile1, outFile2;
	inFile.open(argv[1]);
	outFile1.open(argv[2]);
	outFile2.open(argv[3]);
	RadixSort radix;
	
	radix.firstReading(inFile);
	inFile.close();
	inFile.open(argv[1]);
	radix.loadStack(inFile, outFile2);
	radix.Sort(outFile1, outFile2);
	
	inFile.close();
	outFile1.close();
	outFile2.close();
	return 0;
}