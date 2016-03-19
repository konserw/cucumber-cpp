#include <cucumber-cpp/internal/drivers/QtTestDriver.hpp>

#include <QtTest/QtTest>
#include <QString>
#include <QTextStream>
#include <QTemporaryFile>

#include <QObject>

namespace cucumber {
namespace internal {

class QtTestObject: public QObject
{
    Q_OBJECT

public:
    QtTestObject(QtTestStep* step)
    { Step = step; }
    virtual ~QtTestObject() {}

private:
    QtTestStep* Step;

private slots:
    void test()
    { Step->body(); }
};


const InvokeResult QtTestStep::invokeStepBody() {

    QTemporaryFile file;
    QString fileName;
    if (file.open()) {
        fileName = file.fileName();
    }
    file.close();

    QtTestObject test(this);
    int returnValue = QTest::qExec(&test, QStringList() << "test" << "-o" << fileName);
    if(returnValue == 0)
        return InvokeResult::success();
    else
    {
        file.open();
        QTextStream ts(&file);
        return InvokeResult::failure(ts.readAll().toLatin1());
    }
}

}
}

#include "QtTestDriver.moc"
