#include <unistd.h>
#include <getopt.h>
#include <libgen.h> // FIXME

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <map>

#include "cppunit-header.h"

typedef std::map<std::string, CppUnit::Outputter *> OutputterMap;
typedef std::map<std::string, CppUnit::TestListener *> ListenerMap;

//////////////////////////////////////////////////////////////////////
void usage()
{
}

//////////////////////////////////////////////////////////////////////
void dump(CppUnit::Test * test)
{
	if (test)
	{
		std::cout << test->getName() << std::endl;

		if (test->getChildTestCount())
		{
			for (int i = 0; i < test->getChildTestCount(); i++)
			{
				dump(test->getChildTestAt(i));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
CppUnit::Test * find(CppUnit::Test * root, const std::string & name)
{
	CppUnit::Test * found = nullptr;

	if (root)
	{
		if (name == root->getName())
			found = root;
		else if (root->getChildTestCount())
		{
			for (int i = 0; (!found) && i < root->getChildTestCount(); ++i)
				found = find(root->getChildTestAt(i), name);
		}
	}

	return found;
}


//////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	CppUnit::TestResult result;
	CppUnit::TestResultCollector collector;

	result.addListener(&collector);

	OutputterMap allOutputters{
		{ "compiler", new CppUnit::CompilerOutputter(&collector, std::cout)},
		{ "text", new CppUnit::TextOutputter(&collector, std::cout) },
		{ "xml", new CppUnit::XmlOutputter(&collector, std::cout) },
		{"none", nullptr },
	};

	ListenerMap allListeners{
		{ "dots", new CppUnit::TextTestProgressListener() },
		{ "brief", new CppUnit::BriefTestProgressListener() },
		{ "none", nullptr },
	};

	CppUnit::Outputter * outputter = allOutputters.find("compiler")->second;
	CppUnit::TestListener * listener = allListeners.find("dots")->second;

	char flag = 0;

	std::string runTest = basename(argv[0]);

	if (!find(CppUnit::TestFactoryRegistry::getRegistry().makeTest(), runTest))
	{
		runTest = "All Tests";
	}

	int repeat = 1;

	while ((flag = getopt(argc, argv, "r:t:o:p:lh")) != -1)
	{
		switch(flag)
		{
		case 'r':
			repeat = atoi(optarg);
			break;

		case 'o':
			{
			OutputterMap::const_iterator it = allOutputters.find(optarg);

			if (it == allOutputters.end())
			{
				std::cerr << "Unknown outputter: " << optarg << std::endl;
				return 1;
			}

			outputter = it->second;
			}
			break;

		case 'p':
			{
			std::string progress(optarg);

			ListenerMap::const_iterator it = allListeners.find(optarg);

			if (it == allListeners.end())
			{
				std::cerr << "Unknown listener: " << optarg << std::endl;
				return 1;
			}

			listener = it->second;
			}
			break;

		case 'l':
			dump(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
			return 0;
			break; // not reached

		case 't':
			runTest = optarg;
			break;

		case 'h':
		default:
			usage();
			return 1;
			break; // not reached
		}

	}

	if (listener != nullptr)
		result.addListener(listener);

	CppUnit::Test * run = find(CppUnit::TestFactoryRegistry::getRegistry().makeTest(), runTest);

	if (run == nullptr)
	{
		std::cerr << "Unknown test case: " << runTest << std::endl;
		return 1;
	}

	CppUnit::TestRunner runner;

	runner.addTest(run);

	for (int i = 0; i < repeat; i++) {
		runner.run(result);
	}

	if (outputter) outputter->write();

	return collector.testErrors() + collector.testFailures();
}

