
#include "yqlua/QsMovie.h"
QsMovie::QsMovie(const std::string& filepath)
{
    if (filepath.empty())
        std::cout << "QsMovie filepath is empty" << std::endl;
    m_movie = new QMovie(QString::fromStdString(filepath));
}

QsMovie::~QsMovie()
{ 
}
void QsMovie::regist(kaguya::State& T)
{
    T["QsMovie"].setClass(kaguya::UserdataMetatable<QsMovie>()
        .setConstructors<QsMovie(const std::string&)>()
        .addFunction("setSpeed",&QsMovie::setSpeed)
        .addFunction("start", &QsMovie::start)
    );
}
void QsMovie::setSpeed(int speed)
{
    m_movie->setSpeed(speed);
}

void QsMovie::start()
{
    m_movie->start();
}