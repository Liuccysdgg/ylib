
#include "yqlua/QsTimer.h"
QsTimer::QsTimer()
{
    QObject::connect(this, &QTimer::timeout, [&]() {
        if (!m_onTimeoutCallback.isNilref())
            m_onTimeoutCallback();
    });
}

QsTimer::~QsTimer()
{ 
}
void QsTimer::regist(kaguya::State& T)
{
    T["QsTimer"].setClass(kaguya::UserdataMetatable<QsTimer>()
        .setConstructors<QsTimer()>()
        .addFunction("onTimeout",&QsTimer::onTimeout)
        .addFunction("stop", &QsTimer::stop)
        .addFunction("start", &QsTimer::start)
    );
}
void QsTimer::start(int msec)
{
    QTimer::start(msec);
}
