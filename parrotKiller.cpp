#include <CQEVE.h>
#include <CQLogger.h>
#include <parrotKiller.h>

//#include "APPINFO.h"

#include <ctime>
#include <queue>
#include <list>
#include <cmath>
#include <map>
#include <set>
#include <string>

enum banMode { cnst, ex };

using namespace std;
using namespace CQ;

class ParrotGroup
{
private:
	banMode bm;
	long long groupNum;
	string repeatingMsg;
	list<string> record;
	int counter;
	int banTime;
	bool enabled;

public:


};