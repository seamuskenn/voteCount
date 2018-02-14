#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <math.h>
using namespace std;
ifstream file("voting.txt");

class Candidate{
	public:
		void setName(string name);
		string getName(void);

		void setParty(string party);
		string getParty(void);

		Candidate(string name, string party); //constructor


	private:
		string candName;
		string candParty;
};
Candidate::Candidate(string name, string party)
{
	candName = name;
	candParty = party;
	cout<< "Candidate "<< name<< " has been created as a member of ";
	cout<< party<< " party\n";
}
void Candidate::setName(string name)
{
	cout<< candName;
	candName = name;
	cout<< " updated to "<< candName;
}
string Candidate::getName()
{
	return candName;
}
void Candidate::setParty(string party)
{
	cout<< candParty;
	candParty = party;
	cout<< " updated to "<< candParty;
}
string Candidate::getParty()
{
	return candParty;
};

class BallotPaper{
	public:
		BallotPaper(int candCount, string line);
		
		void setPreference(int pref, int cand);
		int getPreference(int pref);
		vector<int> getBallot();
	
	private:
		vector<int> prefs;
};
BallotPaper::BallotPaper(int candCount, string line)
{
	int pref, split, size = candCount;
	prefs.resize(size);
	for(int i=0; i<size; i++)
	{
		stringstream(line)>> pref;
		split = line.find(",");
		prefs[pref-1] = i;
		line = line.substr(split+1);
	}
}
void BallotPaper::setPreference(int pref, int cand)
{
	prefs[pref-1] = cand;
}
int BallotPaper::getPreference(int pref)
{
	return prefs[pref];
}
vector<int> BallotPaper::getBallot()
{
	return prefs;
};

int voteCounter(vector<BallotPaper> votes, vector<Candidate> nominees)
{
	int large, elim, size = (int)nominees.size();
	bool check;
	int trans[size];
	bool valid[size];
	vector<int> small;
	vector<BallotPaper> cast[size];
	//cast.set_size(size);
	srand(time(NULL));
	for(int i=0; i<size; i++)
	{
		valid[i] = true;
		trans[i] = 0;
	}
	for(int i=0; i<(int)votes.size(); i++)
	{
		cast[votes[i].getPreference(0)].push_back(votes[i]);
	}
	cout<< endl;
	for(int i=1; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			if(valid[j])
			{
				cout<< nominees[j].getName()<< ": ";
				cout<< cast[j].size()<< " vote(s)\n";
			}
		}
		check = true;
		for(int j=0; j<size; j++)
		{
			if(check && valid[j])
			{
				check = false;
				small.push_back(j);
				large = j;
			}
			else if(valid[j])
			{
				if(cast[small[0]].size() > cast[j].size())
				{
					small.clear();
					small.push_back(j);
				}
				else if(cast[small[0]].size() == cast[j].size())
				{
					small.push_back(j);
				}
				if(cast[large].size() < cast[j].size())
				{
					large = j;
				}
			}
		}
		if(cast[large].size()*2 > votes.size())
		{
			cout<< endl<< nominees[large].getName();
			cout<< " has a majority with "<< cast[large].size();
			cout<< " votes out of "<< votes.size()<< " votes cast\n";
			return large;
		}
		if(small.size() > 1)
		{
			cout<< "\ntie for fewest votes ("<< cast[small[0]].size();
			cout<< ") between:\n";
			for(int j=0; j<(int)small.size(); j++)
			{
				cout<< nominees[small[j]].getName()<< endl;
			}
		}
		elim = small[rand()%small.size()];
		small.clear();
		valid[elim] = false;
		cout<< "\ncandidate "<< nominees[elim].getName();
		cout<< " eliminated\n";
		for(int j=0; j<(int)cast[elim].size(); j++)
		{
			for(int k=1; k<size; k++)
			{
				if(valid[cast[elim][j].getPreference(k)])
				{
					trans[cast[elim][j].getPreference(k)]++;
					cast[cast[elim][j].getPreference(k)].push_back(cast[elim][j]);
					break;
				}
			}
		}
		for(int j=0; j<size; j++)
		{
			if(valid[j] && trans[j])
			{
				cout<< trans[j]<< " vote(s) transferred to ";
				cout<< nominees[j].getName()<< endl;
			}
			trans[j] = 0;
		}
		cout<< endl;
	}
	for(large=0; large<size; large++)
	{
		if(valid[large])
			break;
	}
	cout<< "all candidates except "<< nominees[large].getName();
	cout<< " have been eliminated"<< endl;
	return large;
}

int sizeCalc(int numCands)//calculates max number of nodes needed
{
	if(numCands == 2)
		return 3;
	return(numCands * sizeCalc(numCands-1) + 1);
}	

class Node
{
	public:
		Node(int size);
		Node();
		int count;
		vector<Node*> children;
};
Node::Node()
{
	count = 0;
}
Node::Node(int size)
{
	children.resize(size);
	for(int i=0; i<size; i++)
	{
		children[i] = 0;
	}
	count = 0;
};
class treeCount
{
	public:
		treeCount(vector<Candidate> nominees);
		void add(string line);
		bool overHalf();
		void elimination();
		void redist(Node* current, int elim, int level);
		
		Node *head;
		vector<Candidate> nom;
};
treeCount::treeCount(vector<Candidate> nominees)
{
	Node *start = new Node(nominees.size());
	head = start;
	nom = nominees;
}
void treeCount::add(string line)
{
	int pref, split, cast = head->children.size();
	int vote[cast];
	for(int i=0; i<cast; i++)
	{
		stringstream(line)>> pref;
		split = line.find(",");
		vote[pref-1] = i;
		line = line.substr(split+1);
	}
	Node *current = head;
	for(int i=0; i<cast; i++)
	{
		if(!current->children[vote[i]])
		{
			Node *new_node = new Node(cast-i-1);
			current->children[vote[i]] = new_node;
		}
		for(int j=i+1; j<cast; j++)
		{
			if(vote[j] > vote[i])
			{
				vote[j]--;
			}
		}
		current->count++;
		current = current->children[vote[i]];
	}
	current->count++;
}
bool treeCount::overHalf()
{
	for(int i=0; i<(int)head->children.size(); i++)
	{
		if(head->count < head->children[i]->count *2)
			return true;
	}
	return false;
}
void treeCount::elimination()
{
	vector<int> small;
	int large, elim;
	bool set = false;
	srand(time(NULL));
	for(int i=0; i<(int)nom.size(); i++)
	{
		cout<<head->children[i]->count<<",";
		if(head->children[i])
		{
			if(!set)
			{
				small.push_back(i);
				large = i;
				set = true;
				cout<<"A\n";
			}
			else if(head->children[i]->count < head->children[small[0]]->count)
			{
				small.clear();
				small.push_back(i);
				cout<<"B\n";
			}
			else if(head->children[i]->count == head->children[small[0]]->count)
			{
				small.push_back(i);
				cout<<"C\n";
			}
			else if(head->children[i]->count > head->children[large]->count)
			{
				large = i;
				cout<<"D\n";
			}
		}
	}
	cout<<"\na"<<small[0]<<"b"<<large<<endl;
	if(small.size() > 1)
	{
		cout<<"Tie for fewest votes (";
		cout<< head->children[small[0]]->count <<") between:\n";
		for(int i=0; i<(int)small.size(); i++)
		{
			cout<<nom[small[i]].getName()<< endl;
		}
	}
	elim = small[rand()%small.size()];
	cout<< nom[elim].getName()<< " has been eliminated"<< endl;
	redist(head, elim, 0);
}
void treeCount::redist(Node* current, int elim, int level)
{
	for(int i=0; i < (int)nom.size() - level; i++)
	{
		if(current->children[i])
		{
			if(i == elim)
			{
				//do stuff
			}
			else if(i < elim)
			{
				redist(current->children[i], elim-1, level+1);
			}
			else
			{
				redist(current->children[i], elim, level+1);
			}
		}
	}
}

int main()
{
	string line, name, party;
	int split;
	//int comp, hold, end, len;
	vector<Candidate> nominees;
	vector<BallotPaper> votes;
	while(getline(file, line))
	{
		if(line == "")
			break;
		split = line.find(";");
		name = line.substr(0, split);
		split++;
		party = line.substr(split);
		Candidate temp(name, party);
		nominees.push_back(temp);
	}
	/*comp = sizeCalc((int)nominees.size());
	hold = file.tellg();		//calculates total number of votes cast
	file.seekg(0, file.end);	//by getting the number of characters
	end = file.tellg();			//remaining to be read and dividing by
	file.seekg(hold);			//the number of characters per vote
	end -= hold;
	len = (int)nominees.size() * 2;
	for(int i=1; (int)nominees.size() > pow(10,i)-1; i++)
	{
		//increases line length as needed to allow for extra characters
		//caused by multiple digit number of candidates
		len += (int)nominees.size() - (pow(10,i)-1);
	}
	end /= len;*/
	//if(comp > end)	//regular implementation with BallotPapers
	if(true)
	{
		while(getline(file, line))
		{
			BallotPaper tempo(nominees.size(), line);
			votes.push_back(tempo);
		}
		nominees[0] = nominees[voteCounter(votes, nominees)];
	}
	else//implementation with tree
	{
		treeCount *tree = new treeCount(nominees);
		while(getline(file, line))
		{
			tree->add(line);
		}
		cout<<tree->head->count<<"abd"<<endl;
		//while(!tree->overHalf())
		{
			tree->elimination();
		}
		
	}
	cout<< nominees[0].getName()<< " wins!";
}
