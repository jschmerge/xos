#include <unistd.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <map>

#include "cppunit-header.h"

namespace config {
	bool verbose = false;
};

typedef std::map<std::string, std::shared_ptr<CppUnit::Outputter>>
          OutputterMap;
typedef std::map<std::string, std::shared_ptr<CppUnit::TestListener>>
          ListenerMap;

//////////////////////////////////////////////////////////////////////
void usage(const char * name)
{
	std::cerr << "usage: " << name
	          << " "
	             "[-r repeatnumber] "
	             "[-t testname] "
	             "[-o {compiler|text|xml|none}] "
	             "[-p {dots|brief|verbose|none}] [-l] [-h]"
	          << std::endl;
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

	using std::make_shared;
	using CppUnit::CompilerOutputter;
	using CppUnit::TextOutputter;
	using CppUnit::XmlOutputter;
	using CppUnit::TextTestProgressListener;
	using CppUnit::BriefTestProgressListener;
	using CppUnit::BriefTestProgressListener;

	OutputterMap allOutputters{
		{ "compiler", make_shared<CompilerOutputter>(&collector, std::cout)},
		{ "text", make_shared<TextOutputter>(&collector, std::cout) },
		{ "xml", make_shared<XmlOutputter>(&collector, std::cout) },
		{"none", nullptr },
	};

	ListenerMap allListeners{
		{ "dots", std::make_shared<CppUnit::TextTestProgressListener>() },
		{ "brief", std::make_shared<CppUnit::BriefTestProgressListener>() },
		{ "verbose", std::make_shared<CppUnit::BriefTestProgressListener>() },
		{ "none", nullptr },
	};

	std::shared_ptr<CppUnit::Outputter> outputter =
		allOutputters.find("compiler")->second;
	std::shared_ptr<CppUnit::TestListener> listener =
		allListeners.find("dots")->second;

	char flag = 0;

	std::string runTest = argv[0];
	std::string::size_type n = 0;

	if ((n = runTest.find_last_of('/')) != std::string::npos)
		runTest = runTest.substr(n + 1);

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

			if (progress == "verbose")
			{
				progress = "brief";
				config::verbose = true;
			}

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
			usage(argv[0]);
			return 1;
			break; // not reached
		}

	}

	if (listener != nullptr)
		result.addListener(listener.get());

	CppUnit::Test * run =
		find(CppUnit::TestFactoryRegistry::getRegistry().makeTest(), runTest);

	if (run == nullptr)
	{
		std::cerr << "Unknown test case: " << runTest << std::endl;
		return 1;
	}

	CppUnit::TestRunner runner;

	runner.addTest(run);

	for (int i = 0; i < repeat; i++)
	{
		runner.run(result);
	}

	if (outputter) outputter->write();

	allOutputters.clear();
	allListeners.clear();

	return collector.testErrors() + collector.testFailures();
}
