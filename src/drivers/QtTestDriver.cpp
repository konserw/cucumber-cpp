#include <cucumber-cpp/internal/drivers/QtTestDriver.hpp>
#include "cucumber-cpp/internal/utils/qtCapture.hpp"
#include <QtConcurrent/QtConcurrent>
#include <QtTest/QtTest>

namespace cucumber {
namespace internal {

class QtTestObject: public QObject {
    Q_OBJECT
public:
    QtTestObject(QtTestStep* qtTestStep): step(qtTestStep) {
    }
    virtual ~QtTestObject() {
    }

protected:
    QtTestStep* step;

private slots:
    void test() const {
        step->body();
    }
};


InvokeResult runTestInAnotherThread(QtTestStep* step) {
    QtTestObject testObject(step);

    qtCapture::Init();
    qtCapture::BeginCapture();
    int returnValue = QTest::qExec(&testObject, 0, NULL);
    qtCapture::EndCapture();

    if(returnValue == 0)
        return InvokeResult::success();
    else
        return InvokeResult::failure(qtCapture::GetCapture());

}

const InvokeResult QtTestStep::invokeStepBody() {
    return QtConcurrent::run(runTestInAnotherThread, this).result();
}

}
}

#include "QtTestDriver.moc"
