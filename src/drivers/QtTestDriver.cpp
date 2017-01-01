#include <cucumber-cpp/internal/drivers/QtTestDriver.hpp>

#include <QtTest/QtTest>
#include <QString>
#include <QTextStream>
#include <QTemporaryFile>
#include <QBuffer>
#include <QObject>
#include <QFile>
#include <cstdio>

namespace cucumber {
namespace internal {

class QtTestObject: public QObject {
    Q_OBJECT
public:
    QtTestObject(QtTestStep* qtTestStep): step(qtTestStep) {}
    virtual ~QtTestObject() {}

protected:
    QtTestStep* step;

private slots:
    void test() const {
        step->body();
    }
};


const InvokeResult QtTestStep::invokeStepBody() {
    QtTestObject test(this);

#if defined(_POSIX_VERSION)
    const int maxLength = 4096;
    char buffer[maxLength+1] = {0};
    int out_pipe[2];
    int saved_stdout;

    saved_stdout = dup(STDOUT_FILENO);  /* save stdout */
    if( pipe(out_pipe) != 0 ) {          /* make a pipe */
        exit(1);
    }
    dup2(out_pipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
    close(out_pipe[1]);

    int returnValue = QTest::qExec(&test, 0, NULL);

    dup2(saved_stdout, STDOUT_FILENO);  /* reconnect stdout */

    if(returnValue == 0)
        return InvokeResult::success();
    else
    {
        read(out_pipe[0], buffer, maxLength); /* read from pipe into buffer */
        return InvokeResult::failure(buffer);
    }
#else
    QTemporaryFile file;
    QString fileName;
    if (file.open()) {
        fileName = file.fileName();
    }
    file.close();

    int returnValue = QTest::qExec(&test, QStringList() << "test" << "-o" << fileName);
    if(returnValue == 0)
        return InvokeResult::success();
    else
    {
        file.open();
        QTextStream ts(&file);
        return InvokeResult::failure(ts.readAll().toLatin1());
    }
#endif
}

}
}

#include "QtTestDriver.moc"
