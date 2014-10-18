#ifndef GUARD_CPPUNIT_HEADER_H
#define GUARD_CPPUNIT_HEADER_H 1

///
/// Convenience header for test harness. Include this in test source instead
/// these headers directly... This allows us to stick workarounds here
///
#include <iostream>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestSuccessListener.h>
#include <cppunit/TextTestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/XmlOutputter.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

//
// Redefinition to use unique_ptr instead of auto_ptr - unchanged otherwise
//
#undef CPPUNIT_TEST_SUITE_END
#define CPPUNIT_TEST_SUITE_END() \
	} \
	static CPPUNIT_NS::TestSuite *suite()                                      \
	{                                                                          \
		const CPPUNIT_NS::TestNamer &namer = getTestNamer__();                 \
		std::unique_ptr<CPPUNIT_NS::TestSuite> suite(                          \
			new CPPUNIT_NS::TestSuite( namer.getFixtureName() ));              \
		CPPUNIT_NS::ConcretTestFixtureFactory<TestFixtureType> factory;        \
		CPPUNIT_NS::TestSuiteBuilderContextBase context( *suite.get(),         \
		                                                 namer,                \
		                                                 factory );            \
		TestFixtureType::addTestsToSuite( context );                           \
		return suite.release();                                                \
	}                                                                          \
 private: /* dummy typedef so that the macro can still end with ';'*/          \
	typedef int CppUnitDummyTypedefForSemiColonEnding__
// End of Macro

namespace config {
	extern bool verbose;
};

#endif // GUARD_CPPUNIT_HEADER_H
