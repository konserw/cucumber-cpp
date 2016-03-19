#include <QtTest>
#include <cucumber-cpp/autodetect.hpp>

#include "../../utils/DriverTestRunner.hpp"

using namespace cucumber;

THEN(SUCCEED_MATCHER) {
    ScenarioScope<SomeContext> ctx;
    QVERIFY(true);
}

THEN(FAIL_MATCHER) {
    ScenarioScope<SomeContext> ctx;
    QVERIFY(false);
}

THEN(PENDING_MATCHER_1) {
    pending();
}

THEN(PENDING_MATCHER_2) {
    pending(PENDING_DESCRIPTION);
}

using namespace cucumber::internal;

class QtTestSteps : public QObject {
Q_OBJECT
public:
    QtTestSteps() {
        isInitialized = false;
        isCleanedUp = false;
    }
    bool isInitialized;
    bool isCleanedUp;
private slots:
    void initTestCase()
    { isInitialized = true; }
    void myFirstTest()
    { QVERIFY(1 == 1); }
    void cleanupTestCase()
    { isCleanedUp = true; }
};

class QtTestDriverTest : public DriverTest {
public:
    virtual void runAllTests() {
        stepInvocationInitsQtTest();
        DriverTest::runAllTests();
    }

private:
    void stepInvocationInitsQtTest() {
        QtTestSteps framework;
        QCOMPARE(framework.isInitialized, false);
        QCOMPARE(framework.isCleanedUp, false);
        QTest::qExec(&framework);
        QCOMPARE(framework.isInitialized, true);
        QCOMPARE(framework.isCleanedUp, true);
    }
};

int main() {
    QtTestDriverTest test;
    return test.run();
}

#include "QtTestDriverTest.moc"
