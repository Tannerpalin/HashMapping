// Project 4 - CS315 with Dr.Raphael Finkel - Word Counter
// Author: Tanner Palin
// Sources: geeksforgeeks.org was extremely helpful with teaching me how to use and implement vectors.
// Sources: geeksforgeeks.org also helped me firmly grasp the idea and implementation of external chaining.
// Sources: Will Shapiro for helping me come up with an efficient hashing equation.
// Sources: www.thepolyglotdeveloper.com helped me implement QuickSort using vectors when sorting occurences.
// Purpose: This program takes in the contents from a file and hashes them in a way that helps you
// determine the commonly used words in a given file.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm> 
// Algorithm is only used to reverse the vector after sorting the second time.
// This vector must be reversed since I abstracted my QuickSort function to work
// for each instance of sorting that is needed.
// I could have probably found a way to manually reverse but that is besides the point of this project.

using namespace std;

struct chainLink {  // Struct to hold a specific word, as well as its metadata.
    string wordValue; // The actual word.
    int nodeInfo[2]; // Zero-Index stores the number of occurences, One-Index holds hash value.
    struct chainLink *next; // Points to next link in chain.
};

int hashMe(string wordToHash, int modNum) { // Returns an index after hashing that can be used to insert.
    int hashIndex = 0;
    for(unsigned int j = 0; j <= wordToHash.length(); j++) {
        hashIndex += (int)wordToHash[j];
    }
    hashIndex = hashIndex % modNum;
    return hashIndex;
}

// Check hash array for collision with new word. If collision, need to externally chain at hash index
int collisionDetect(struct chainLink hashArray[], struct chainLink *newWord, int hashSize) {

    if(hashArray[newWord->nodeInfo[1]].wordValue != "") {
        // Collision! Must check and see if word is in chain.
       
        struct chainLink *temp = new struct chainLink;
        struct chainLink *little;
        temp = hashArray[newWord->nodeInfo[1]].next;
    
        //Check to see if first word in chain is the word we are inserting.
        if(hashArray[newWord->nodeInfo[1]].wordValue.compare(newWord->wordValue) == 0) {
            hashArray[newWord->nodeInfo[1]].nodeInfo[0] += 1; // If same word then increment occurences.
            return 0;
        }
        else{ // We must traverse chain and insert/denote occurence where necessary.
            if(temp == NULL) {
                hashArray[newWord->nodeInfo[1]].next = newWord;
                return 0;
            }

            while(temp != NULL) { // Travel down chain.
            
            if(temp->wordValue.compare(newWord->wordValue) == 0) {
                temp->nodeInfo[0] += 1; // If same word then increment occurences.
                return 0;
            }
                little = temp;
                temp = temp->next;
            }
        little->next = newWord;
        return 0;
        }
    }
    else{ return 1;}
}

// Inserts new word into hash array, checks for collisions first.
void insertUnsorted(struct chainLink hashArray[], struct chainLink * newWord, int hashSize) {
    int isCollision = collisionDetect(hashArray, newWord, hashSize);

    if(isCollision != 0) {  // If no collision, we insert normally.
        hashArray[newWord->nodeInfo[1]] = *newWord;
    }
    return;
}

// Takes vector of words and puts them into hash array (unsorted).
void vectorToArray(struct chainLink hashArray[], vector<string> &wordsIn, int numWords) {
    int hashIndex;
   
    for(auto q = wordsIn.begin(); q != wordsIn.end(); q++) {
        hashIndex = hashMe(*q, numWords);
        struct chainLink * newLink = new struct chainLink;
        newLink->wordValue = *q;
        newLink->next = NULL;               // Populating struct before entering into hash array.
        newLink->nodeInfo[0] = 1;
        newLink->nodeInfo[1] = hashIndex;   
        insertUnsorted(hashArray, newLink, numWords);
    }
    return;
}

// Initializing Hash Array to allow for easier collision detection.
void initializeHashArray(struct chainLink hashArray[], int numWords) {
    for(int p = 0; p < numWords; p++) {
        hashArray[p].next = NULL;
        hashArray[p].nodeInfo[0] = -1; // Zero index for number of word occurences.
        hashArray[p].nodeInfo[1] = -1; // One index for hash index value.
        hashArray[p].wordValue = "";
    }
}

// Offloading the hash array into a single vector of structs for easier sorting.
void offLoad(struct chainLink hashArray[], vector<struct chainLink>& words, int numWords) {
    struct chainLink *tempPtr;

    for(int i = 0; i < numWords; i++) {
        tempPtr = hashArray[i].next;
        if(hashArray[i].wordValue != "") {
            words.push_back(hashArray[i]);
        }
        while(tempPtr != NULL) {
            words.push_back(*tempPtr);
            tempPtr = tempPtr->next;
        }
    }
}

// Performing a single partition. Which = 0, alphabetical comparision. Which = 1, number of occurences comparison.
// I implemented this partition using Hoare's method and learned about it from polyglotdeveloper.com
int partitionByOccur(vector<struct chainLink> &words, int left, int right, int which) {
    int pivot = left + (right - left) / 2;
    int pivotVal = words[pivot].nodeInfo[0];
    string pivotWord = words[pivot].wordValue;
    int l = left;
    int r = right;
    struct chainLink tempVal;
    
    while(l <= r) { // Partitioning using Hoare's method.
        
        if(which == 1) {    // If 1, sort by occurences.
            while(words[l].nodeInfo[0] < pivotVal) {
            l++;
            }
            while(words[r].nodeInfo[0] > pivotVal) {
            r--;
            }
        }
        else if(which == 0) { // If 0, sort by alphabet.
            while(words[l].wordValue < pivotWord) {
                l++;
            }
            while(words[r].wordValue > pivotWord) {
                r--;
            }
        }
        
        if(l <= r) {
            tempVal = words[l];
            words[l] = words[r];
            words[r] = tempVal;
            l++;
            r--;
        }
    }
    return l;
}

// Which = 0, sort by alphabet. Which = 1, sort by occurences.
void quickSort(vector<struct chainLink> &words, int left, int right, int which) {
    if(left < right) {
        int pivot = partitionByOccur(words, left, right, which);
        quickSort(words, left, pivot - 1, which);
        quickSort(words, pivot, right, which);
    }
    return;
}

// Sorting out occurence ties.
void sortTies(vector<struct chainLink> &words, int numWords) {
   int numRight = 0;
   int numLeft = 0;

   for(auto i = words.begin(); i != words.end(); i++) {
        numRight++;
        // Alphabetically QuickSorting the sections of the vector that have the same number of occurences.
        if((words[numLeft].nodeInfo[0] != words[numRight].nodeInfo[0]) && ((numRight - numLeft) > 0)) {
            quickSort(words, numLeft, numRight-1, 0);
            numLeft = numRight;
        }
   }
}

int main(int argc, char *argv[]) {
    if(argc != 1) {
        cout << "Usage: ./wordCount < fileName\n(Redirect standard input to text file)\n";
        return 0;
    }

    vector<string> words;
    string wordIn;
    int wordNum = 0;
   
    while(cin >> wordIn) {          // Populating initial vector with words.
        words.push_back(wordIn);
        wordNum = wordNum + 1;
    }

    struct chainLink wordArray[wordNum]; // Array to be hashed into.

    initializeHashArray(wordArray, wordNum);

    vectorToArray( wordArray, words, wordNum);  // Uses string vector to populate hash array.

    vector<struct chainLink> mostUsed;
    offLoad(wordArray, mostUsed, wordNum); // Offloading from hash map into vector.
   
    quickSort(mostUsed, 0, mostUsed.size() - 1, 0);

    for( auto iter = mostUsed.begin(); iter != mostUsed.end(); ++iter) {
        cout << iter->wordValue << " " << iter->nodeInfo[0] << endl;
    }

    quickSort(mostUsed, 0, mostUsed.size() - 1, 1);
    
    cout << endl; // Line space between alphabetical and most frequent :)

    reverse(mostUsed.begin(), mostUsed.end()); // Reversing due to my QuickSort abstraction.

    sortTies(mostUsed, wordNum); // Sorting the occurences ties alphabetically.

    for( auto it = mostUsed.begin(); it != mostUsed.end(); ++it) {
        cout << it->wordValue << " " << it->nodeInfo[0] << endl;
    }

    return 0;
}