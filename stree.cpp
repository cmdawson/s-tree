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

struct parse_node 
{
    unsigned term;    
    parse_node* parent;
    string parse_string;
    list<parse_node> children;
};

// Parse a description of a single tree.
void parse_stree(parse_node* parent)
{
    if (parent->parse_string.empty())
	return;

    string::iterator i0 = parent->parse_string.begin(), i1=i0;

    // move forward to capture the node label, if there is one    
    while (*(++i1) != '(')
	;
    string sval = string(i0,i1);

    unsigned ival = 0;
    if (sval != "")
	ival = stoul(sval);
    parent->term = ival;
    i0 = i1;
   
    // Extract child parse trees ( ... ), and  
    int open = 1;
    while (++i1 != parent->parse_string.end())
    {
	if (*i1 == '(')
	    open++;
	else if (*i1 == ')')
	    open--;

	if (!open)
	{
	    string schild = string(++i0,i1);
	    if (!schild.empty())
		parent->children.push_back(
			parse_node{0,parent,schild,list<parse_node>()});
	    i0 = i1;
	    i0++;
	}
    }

    if (open != 0)
    {
	//cout << parent->parse_string << endl;
	throw runtime_error("Parenthesis mismatch: " + parent->parse_string);
    }

    for (auto& x: parent->children)
	parse_stree(&x);
}


// performs the depth-first traversal and keeps track of the running sum.
struct DepthSum
{
    unsigned _sum, _cumulated;
    bool _success;

    DepthSum(unsigned sum) : _sum(sum), _cumulated(0), _success(false) {} 

    void reset(void) 
    {
	_cumulated = 0;
	_success = false;
    }

    bool traverse(parse_node* p)
    {
	_cumulated += p->term;

	if (p->children.empty())
	{
	    if (_cumulated == _sum)
		_success = true;
	}

	for (auto& ci: p->children)
	    traverse(&ci);

	_cumulated -= p->term;

	return _success;
    }
};



// Read the next instance of the problem in the filestream, setting the 
// sum target and filling buf with a (whitespace free) description of
// the tree that can be parsed by  parse_stree above
bool read_next(fstream& fs, unsigned& tgt, string& buf)
{
    //fs >> tgt;
    buf.erase();
    
    if (fs.bad())
	return false;

    // scan to first opening bracket, filling buf
    char c = fs.get();
    while (fs.good() && c != '(')	
    {
	if (not isspace(c))
	    buf += c;
	c = fs.get();
    }

    if (buf.empty())
	return false;

    // Attempt to read integer from buf
    tgt = stoul(buf);
    buf.erase();
    c = fs.get();
    unsigned open = 1;
    
    // scan to matching closing bracket, filling buf
    do 
    {
	if (c == ')')
	    open--;
	else if (c == '(')
	    open++;
    
	if (open==0)
	    break;
	
  	if (not isspace(c))
	    buf += c;

	c = fs.get();
    } while (fs.good());

    return open == 0;
}



int main(int argc, char* argv[])
{
    if (argc != 2)
    {
	cerr << "Usage: stree file.txt" << endl;    
	return 1;
    }

    string buf;
    fstream fs(argv[1], ios::in);

    fs.seekg(0,fs.end);
    size_t MAXBUF = fs.tellg();
    fs.seekg(0,fs.beg);
    buf.reserve(MAXBUF);

    unsigned target;
    while (fs.good())
    {
	if (read_next(fs, target, buf))
	{
	    parse_node p = {0,0,buf,list<parse_node>()};
	    parse_stree(&p);

	    DepthSum ds(target);

	    if (ds.traverse(&p))
		cout << "yes" << endl;
	    else
		cout << "no" << endl;
	}
    }

    fs.close();
}
