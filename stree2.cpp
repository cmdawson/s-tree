/* Parses trees as per the assignment problem at

   http://www.cs.duke.edu/courses/cps100/fall03/assign/extra/treesum.html

   which would have been easier in lisp. Can handle arbitrary trees. Uses the auto
   keyword so needs --std=c++11 on g++ for example.*/
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <stdexcept>
#include <cctype>

using namespace std;

class STree
{
public:

    struct Node 
    {
	Node* parent;
	unsigned value;
	list<Node> children;

	Node(Node* p, unsigned v=0) : parent(p), value(v) {}
    };

private:

    Node _sentinel;
    unsigned _total;
    bool _success;

    static inline bool NEW_NODE(const char c) { return c == '('; }
    static inline bool END_NODE(const char c) { return c == ')'; }

    void depthFirstSum(Node* n)
    {
	_total += n->value;
	if (n->children.empty())
	{
	    if (_total == target())
		_success = true;
	}
	for (auto& ci: n->children)
	    depthFirstSum(&ci);

	_total -= n->value;
    }

public:


    STree(void) : _sentinel(0) { }

    ~STree(void) {}

    unsigned target(void) 
    {	
	return _sentinel.value;
    }

    void addNode(Node* parent, fstream& fs)
    {
	char c;
	parent->children.push_back(Node(parent));
	Node* current = &(parent->children.back());
	
	while (fs >> noskipws >> c)
	{
	    if (isspace(c))
		continue;

	    if (isdigit(c))
	    {
		string digits(1,c);
		while (isdigit(fs.peek()))
		    digits.push_back(fs.get());
		current->value = stoul(digits);
	    }

	    else if (NEW_NODE(c))
		addNode(current, fs);

	    else if (END_NODE(c))
	    {
		// tidy up, remove null children
		list<Node>::iterator li = current->children.begin();
		while (li != current->children.end())
		{
		    if (not li->value and li->children.empty())
			li = current->children.erase(li);
		    else
			li++;
		}
		return;
	    }

	    else // or you could just ignore it
		throw runtime_error("Unexpected character");
	}
	
	cout << current->value << ", " << parent->value << endl;
	throw runtime_error("Unexpected eof)");
    }


    bool read(fstream &fs)
    {
	// Read the initial target and position for tree description
	string digits;
	char c = fs.get();
	while (not NEW_NODE(c) && fs.good())
	{
	    if (isdigit(c))
		digits.push_back(c);
	    c = fs.get();
	}
	if (not NEW_NODE(c))
	    return false;

	// Attach the entire tree below a _sentinel node (which also stores the
	// target
	unsigned target = stoul(digits);
	_sentinel = Node(0,target);
	addNode(&_sentinel, fs);

	return true;
    }


    bool hasLeafSum(void)
    {
	_total = 0;
	_success = false;

	depthFirstSum(&(_sentinel.children.front()));
	return _success;
    }
};



int main(int argc, char* argv[])
{
    if (argc != 2)
    {
	cerr << "Usage: stree file.txt" << endl;    
	return 1;
    }

    fstream fs(argv[1], ios::in);
    if (fs.bad())
	return 1;

    STree stree;
    while (fs.good())
    {
	bool ok = stree.read(fs);
	if (ok)
	    if (stree.hasLeafSum())
		cout << "Yes" << endl;
	    else
		cout << "No" << endl;
    }
    fs.close();
}
